#include <graphics.h>
#include <windows.h>
#include <conio.h> // For getch()
#include <stdio.h>
#include <stack>
#include <string.h>

using namespace std;

// Define the Disk structure
struct Disk {
    int width;
    int color;
};

class HanoiInterface {
private:
    const int WINDOW_WIDTH;
    const int WINDOW_HEIGHT;
    const int PEG_COUNT;
    int pegX[3];
    const int PEG_HEIGHT;
    const int DISK_HEIGHT;
    const int MOVE_DELAY;

public:
    HanoiInterface(int width = 1280, int height = 720, int move_delay = 2)
        : WINDOW_WIDTH(width), WINDOW_HEIGHT(height), PEG_COUNT(3), PEG_HEIGHT(400),
          DISK_HEIGHT(20), MOVE_DELAY(move_delay) {}

    void initialize() {
        initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Thap Ha Noi");
        pegX[0] = WINDOW_WIDTH / 4;
        pegX[1] = WINDOW_WIDTH / 2;
        pegX[2] = 3 * WINDOW_WIDTH / 4;
    }

    void drawPegs() {
        setcolor(WHITE);
        for (int i = 0; i < PEG_COUNT; ++i) {
            line(pegX[i], WINDOW_HEIGHT - 100, pegX[i], WINDOW_HEIGHT - 100 - PEG_HEIGHT);
            line(pegX[i] - 50, WINDOW_HEIGHT - 100, pegX[i] + 50, WINDOW_HEIGHT - 100);
        }
    }

    void drawDisksAll(stack<Disk> pegs[]) {
        drawPegs();

        for (int i = 0; i < PEG_COUNT; ++i) {
            stack<Disk> temp = pegs[i];
            stack<Disk> reversed;
            while (!temp.empty()) {
                reversed.push(temp.top());
                temp.pop();
            }
            int diskIndex = 0;
            while (!reversed.empty()) {
                Disk d = reversed.top();
                reversed.pop();
                int x = pegX[i];
                int y = WINDOW_HEIGHT - 100 - (diskIndex + 1) * DISK_HEIGHT;
                drawDiskAt(x, y, d);
                diskIndex++;
            }
        }
    }

    void drawDiskAt(int x, int y, Disk d) {
        setfillstyle(SOLID_FILL, d.color);
        bar(x - d.width / 2, y - DISK_HEIGHT, x + d.width / 2, y);
        setcolor(BLACK);
        rectangle(x - d.width / 2, y - DISK_HEIGHT, x + d.width / 2, y);
        setcolor(WHITE);
    }

    void displayCompletionMessage() {
        settextstyle(8, 0, 3);
        outtextxy(WINDOW_WIDTH / 2 - 100, 50, (char *)"Complete!");
        delay(2000);
    }

    void pauseAnimation(int ms = 2) {
        delay(ms);
    }

    int getPegX(int peg) const {
        if (peg >= 0 && peg < PEG_COUNT)
            return pegX[peg];
        return 0;
    }

    int getWindowHeight() const { return WINDOW_HEIGHT; }
    int getDiskHeight() const { return DISK_HEIGHT; }
    int getPegHeight() const { return PEG_HEIGHT; }
    int getMoveDelay() const { return MOVE_DELAY; }

    int promptNumberOfDisks() {
        settextstyle(8, 0, 3);
        char inputText[10] = "";
        int diskNumber = 0;

        while (true) {
            cleardevice();
            outtextxy(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 50, (char*)"Enter number of disks (1-10):");
            outtextxy(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, inputText);

            char c = getch();

            if (c == 13) { // Enter key
                sscanf(inputText, "%d", &diskNumber);
                if (diskNumber >= 1 && diskNumber <= 10) {
                    break;
                } else {
                    outtextxy(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 + 50, (char*)"Invalid number of disks.");
                    delay(1000);
                    strcpy(inputText, "");
                }
            }
            else if (c == 8) { // Backspace key
                if (strlen(inputText) > 0) {
                    inputText[strlen(inputText) - 1] = '\0';
                }
            }
            else if (c >= '0' && c <= '9') {
                if (strlen(inputText) < 9) {
                    int len = strlen(inputText);
                    inputText[len] = c;
                    inputText[len + 1] = '\0';
                }
            }
        }

        return diskNumber;
    }
};

class HanoiLogic {
private:
    int numDisks;
    stack<Disk> pegs[3];
    HanoiInterface &interfaceRef;

public:
    HanoiLogic(int n, HanoiInterface &iface) : numDisks(n), interfaceRef(iface) {}

    void initializeDisks() {
        int diskColor = BLUE;
        for (int i = numDisks; i >= 1; --i) {
            Disk d;
            d.width = 50 + i * 20;
            d.color = diskColor;
            pegs[0].push(d);
        }
        // Vẽ ban đầu không cần cleardevice vì nó sẽ được gọi bởi double buffering sau
        interfaceRef.drawDisksAll(pegs);
        setvisualpage(0); // Hiển thị trang đầu tiên ban đầu
    }

    void animateMoveDisk(int from, int to) {
        if (pegs[from].empty())
            return;

        Disk movingDisk = pegs[from].top();
        pegs[from].pop();

        int startX = interfaceRef.getPegX(from);
        int startY = interfaceRef.getWindowHeight() - 100 - (pegs[from].size()) * interfaceRef.getDiskHeight();
        int endX = interfaceRef.getPegX(to);
        int endY = interfaceRef.getWindowHeight() - 100 - (pegs[to].size()) * interfaceRef.getDiskHeight();
        int liftHeight = interfaceRef.getWindowHeight() - 100 - interfaceRef.getPegHeight() - 50;

        int currentPage = 0;

        // Lift the disk
        for (int y = startY; y > liftHeight; y -= 20) {
            setactivepage(currentPage);
            cleardevice();
            interfaceRef.drawPegs();
            interfaceRef.drawDisksAll(pegs);
            interfaceRef.drawDiskAt(startX, y, movingDisk);
            setvisualpage(currentPage);
            interfaceRef.pauseAnimation(interfaceRef.getMoveDelay());
            currentPage = 1 - currentPage;
        }

        // Move horizontally
        int stepX = (endX > startX) ? 20 : -20;
        for (int x = startX; ; x += stepX) {
            if ((stepX > 0 && x + stepX > endX) || (stepX < 0 && x + stepX < endX)) {
                x = endX;
            }
            setactivepage(currentPage);
            cleardevice();
            interfaceRef.drawPegs();
            interfaceRef.drawDisksAll(pegs);
            interfaceRef.drawDiskAt(x, liftHeight, movingDisk);
            setvisualpage(currentPage);
            interfaceRef.pauseAnimation(interfaceRef.getMoveDelay());
            currentPage = 1 - currentPage;
            if (x == endX) break;
        }

        // Lower the disk
        for (int y = liftHeight; y < endY + interfaceRef.getDiskHeight(); y += 20) {
            setactivepage(currentPage);
            cleardevice();
            interfaceRef.drawPegs();
            interfaceRef.drawDisksAll(pegs);
            interfaceRef.drawDiskAt(endX, y, movingDisk);
            setvisualpage(currentPage);
            interfaceRef.pauseAnimation(interfaceRef.getMoveDelay());
            currentPage = 1 - currentPage;
        }

        pegs[to].push(movingDisk);

        // Sau khi di chuyển xong, vẽ lại toàn bộ đĩa trên cọc
        setactivepage(currentPage);
        cleardevice();
        interfaceRef.drawPegs();
        interfaceRef.drawDisksAll(pegs);
        setvisualpage(currentPage);
    }

    void solveHanoi(int n, int from, int to, int aux) {
        if (n == 0)
            return;
        solveHanoi(n - 1, from, aux, to);
        animateMoveDisk(from, to);
        solveHanoi(n - 1, aux, to, from);
    }

    void execute() {
        solveHanoi(numDisks, 0, 2, 1);
    }
};

int main() {
    HanoiInterface hanoiInterface(1280, 720, 2);
    hanoiInterface.initialize();

    int numDisks = hanoiInterface.promptNumberOfDisks();

    HanoiLogic hanoiLogic(numDisks, hanoiInterface);
    hanoiLogic.initializeDisks();

    hanoiLogic.execute();

    hanoiInterface.displayCompletionMessage();

    getch();
    closegraph();
    return 0;
}
