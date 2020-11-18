# include "iGraphics.h"
#define PI (acos(-1))
/*
// !!!Warning: Do not change screen size / full screen mode to apply mouse move controls properly

Features
BASIC
1. *Initially 5 curves are produced 1 cosine 3 sin and one sum
3. *All curves are traced with balls
4. *All selected tracers speed can be changed with +/- keys
Advance
5. Adding curves by selecting for the first time (Number of total curves cannot be more that 11)
6. Undo all control operations with 'c'  (first letter of 'clear')
7. Remove all curves and tracers with 'r'  (different from undoing operations)
8. *Change frequency and amplitude by mouse control individually
9. *Make curves visible by 'v'
10. *Change frequency by 'f'/'F'
11. *Change Amplitude by 'a'/'A'
12. *Make individual tracers visible/not visible by numbers(fromt 0 - 9) and 's' for sum curve
13. Resume pause animation by 'p'
14. Toggle help/main page by 'h'
15. All operations marked with '*' is selection supported. You can do it by clicking on numbers from screen


*/
// constants can be changed only by the programmer
const int max_h = 600, max_w = 1300, max_size = 11, max_digit = 2, totalpts = 5000;;
const double i_x = 0, i_y = max_h / 2, maxamp = 40, maxspeed = 30;
// toggle options
int showlines = 1, dir[max_size], selected[max_size + 1], visible[max_size + 1], help = 0, ispaused = 0;
// variables for mouse control
int prex = 0, prey = 0, got = -1;
// separated changes arrays so that we can undo all operations
double ampchange[max_size + 1], freqchange[max_size + 1], pdifchange[max_size + 1], iychange[max_size + 1];
// controls
double speed[max_size + 1], circlepos[max_size + 1];
double amplist [max_size + 1] = {
    100,
    150,
    50,
    90,
    50, 60, 70, 80, 90,100
};
double freqlist[max_size + 1] = {
    1,
    2,
    3,
    4,
    5, 6, 7, 8, 9,10
};
double pdiflist[max_size + 1] = {
    0,
    0,
    20,
    30, 40, 50, 60, 70, 80, 10
};
// help texts
void drawhelp () {
    iSetColor(10, 10, 10);
    iFilledRectangle(max_w - 310, 0, 350, max_h + 100);
    iSetColor(255, 255, 255);
    iText(max_w - 300, max_h + 100 - 20, "HELP will be given at BUET ");
    iText(max_w - 300, max_h + 80 - 20, "to those who ask(h to toggle) for it");
    iText(max_w - 300, max_h + 50 - 20, "Click on numbers to add curves and");
    iText(max_w - 300, max_h + 50 - 40, "apply any selected operations");
    iText(max_w - 300, max_h + 50 - 70, "Use mouse click and move to change");
    iText(max_w - 300, max_h + 50 - 90, "frequency and amplitude individually");
    iText(max_w - 300, max_h + 50 - 120, "+/- to change the speed of the tracer");
    iText(max_w - 300, max_h + 50 - 150, "f/F to change frequency from keyboard");
    iText(max_w - 300, max_h + 50 - 180, "a/A to change frequency from keyboard");
    iText(max_w - 300, max_h + 50 - 210, "Y/y to move the curves vertically");
    iText(max_w - 300, max_h + 50 - 240, "X/x to change phase difference");
    iText(max_w - 300, max_h + 50 - 270, "p to pause and resume");
    iText(max_w - 300, max_h + 50 - 300, "r to remove everything");
    iText(max_w - 300, max_h + 50 - 330, "c to clear all effects of operations");
    iText(max_w - 300, max_h + 50 - 360, "v to make curves visible/unvisible");
    iText(max_w - 300, max_h + 50 - 390, "Numbers (0 to 9) or s(for sum) to");
    iText(max_w - 300, max_h + 50 - 410, "make tracer appear/disappear");
    iText(max_w - 300, max_h + 50 - 440, "q to EXIT");
}
// selection system
char stext[max_size + 1][max_digit + 1];
void sreverse(int id) {
    int i = 0;
    for (i = 0; i < (strlen(stext[id]) / 2); i++) {
        char t = stext[id][strlen(stext[id]) - i - 1];
        stext[id][strlen(stext[id]) - i - 1] = stext[id][i];
        stext[id][i] = t;
    }
}
void drawselections () {
    iSetColor(255, 255, 255);
    int i;
    for (i = 0; i < max_size; i++) {
        if (selected[i]) iSetColor(255, 255, 255);
        else iSetColor(100, 100, 100);
        double recx1 = 20 + i * 30, recx2 = 40 + i * 30, recy1 = max_h + 50, recy2 = max_h + 70;
        iRectangle(recx1, recy1, recx2 - recx1, recy2 - recy1);
        double tx = (recx1 + recx2) / 2.0 - strlen(stext[i]) * 3, ty = (recy1 + recy2) / 2.0 - 2.5;
        char c[2] = {'1' + i, '\0'};
        if (i < max_size - 1) iText(tx, ty, stext[i]);
        else iText(tx, ty, "S");
    }
}
//color arrays and function
double red[] = {255,0,255,20,255,153,102,100, 120, 200, 20};
double green[] = {255,0,0,100,51,0,255,200, 100, 102, 255};
double blue[] = {0,255,0,0,153,153,255,255, 20, 30, 80};
double c_red[max_size];
double c_green[max_size];
double c_blue[max_size];
int colors[max_size];
double linered[] = {0,125,0,125,0,150,120,0, 111, 122, 125};
double linegreen[] = {125,125,125,0,112,125,0,113, 15, 115, 125};
double lineblue[] = {0,0,125,110,15,13,0,110, 110, 15, 125};
double c_linered[max_size];
double c_linegreen[max_size];
double c_lineblue[max_size];
void getrgb (int id, int tp, double &thisR, double &thisG, double &thisB) {
    if (tp) {
        thisR = red[colors[id]] + c_red[id],
        thisG = green[colors[id]] + c_green[id],
        thisB = blue[colors[id]] + c_blue[id];
    }
    else {
        thisR = linered[colors[id]] + c_linered[id],
        thisG = linegreen[colors[id]] + c_linegreen[id],
        thisB = lineblue[colors[id]] + c_lineblue[id];
    }
}

// drawing curves

void drawlines () {
    double x_incr = (max_w) / (totalpts/10.0);
    int pt;
    double c_x = 0.0;
    int scl = 0;
    double thisR, thisG, thisB;
    for (pt = 0; pt < totalpts; pt++) {
        double sumx1 = c_x, sumx2 = c_x + x_incr, sumy1 = 0.0, sumy2 = 0.0;
        int cur;
        for (cur = 0; cur < max_size - 1; cur++) {
            if (!selected[cur]) continue;
            double A = (amplist[cur] + ampchange[cur]), w = 2.0 * PI * (freqlist[cur] + freqchange[cur]) / 400.0,
            phi = (pdiflist[cur] + pdifchange[cur]) * PI / 180.0;
            double x1 = c_x, x2 = x1 + x_incr, y1 = A * sin(w * x1 + phi), y2 = A * sin(w * (x2) + phi);
            if (cur == 1) y1 = A * cos(w * x1 + phi), y2 = A * cos(w * (x2) + phi);
            sumy1 += y1, sumy2 += y2;
            y1 += (i_y + iychange[cur]); y2 += (i_y + iychange[cur]);
            getrgb(cur, 0, thisR, thisG, thisB);
            iSetColor(thisR, thisG, thisB);
            iLine(x1, y1, x1 + x_incr, y2);
        }
        c_x += x_incr;
        if (selected[max_size - 1]) {
            getrgb(max_size - 1, 0, thisR, thisG, thisB);
            iSetColor(thisR, thisG, thisB);
            iLine(sumx1, sumy1 + i_y + iychange[max_size - 1], sumx2, sumy2 + i_y + iychange[max_size - 1]);
        }
    }

}

// circle draw and control
void circlechange () {
    int i;
    for (i = 0; i < max_size; i++) {
            if (circlepos[i] + dir[i] * speed[i] > max_w) dir[i] = -1;
            if (circlepos[i] + dir[i] * speed[i] < 0) dir[i] = 1;
            circlepos[i] += dir[i] * speed[i];
    }
}
void drawballs () {
    double thisR, thisG, thisB;
    double sumx = 0.0, sumy = 0.0;
    int cur;
    for (cur = 0; cur < max_size - 1; cur++) {
        if (!selected[cur]) continue;
        if (dir[cur] == 0) dir[cur] = 1;
        double A = (amplist[cur] + ampchange[cur]), w = 2.0 * PI * (freqlist[cur] + freqchange[cur]) / 400.0,
        phi = (pdiflist[cur] + pdifchange[cur]) * PI / 180.0;
        double x = circlepos[cur], y = A * sin(w * x + phi);
        if (cur == 1) y = A * cos (w * x + phi);
        y += (i_y + iychange[cur]);
        if (visible[cur]) {
            getrgb(cur, 1,  thisR, thisG, thisB);
            iSetColor(thisR, thisG, thisB);
            iFilledCircle(x, y, 5);
        }
    }
    if (selected[max_size - 1]) {
        for (cur = 0; cur < max_size - 1; cur++) {
            if (!selected[cur]) continue;
            if (dir[cur] == 0) dir[cur] = 1;
            double A = (amplist[cur] + ampchange[cur]),
            w = 2.0 * PI * (freqlist[cur] + freqchange[cur]) / 400.0,
            phi = (pdiflist[cur] + pdifchange[cur]) * PI / 180.0;
            double x = circlepos[max_size - 1], y = A * sin(w * x + phi);
            if (cur == 1) y = A * cos(w * x + phi);
            sumy += y;
            y += (i_y + iychange[cur]);
        }
        if (visible[max_size - 1]) {
            getrgb(max_size - 1, 1, thisR, thisG, thisB);
            iSetColor(thisR, thisG, thisB);
            if (dir[max_size - 1] == 0) dir[max_size - 1] = 1;
            iFilledCircle(circlepos[max_size - 1], sumy + (i_y + iychange[cur]), 5);
        }
    }
}



/*
	function iDraw() is called again and again by the system.
*/


void iDraw()
{
    iClear();
    if (showlines) {
            iSetColor(100, 100, 100);
            iLine(0, i_y, max_w, i_y);
    }
    drawselections();
    if (showlines) drawlines();
    drawballs();
    if (help) drawhelp();


}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // mouse selection and changing frequency and amplitude
    if (got != -1 && showlines) {
        int cur = got;
        // Change amplitude
        prey -= i_y + iychange[cur];
        double preA = amplist[cur] + ampchange[cur];
        double cury = my - (i_y + iychange[cur]);
        double chng = cury - prey;
        double prew = 2.0 * PI * (freqlist[cur] + freqchange[cur]) / 400.0,
            phi = (pdiflist[cur] + pdifchange[cur]) * PI / 180.0;
        double pretheta = prew * prex + phi;
        if (cur == 1) {
            if (cos(pretheta) == 0) chng = 0;
            if (cos(pretheta) < 0) chng = -chng;
        }
        else {
            if (sin(pretheta) == 0) chng = 0;
            if (sin(pretheta) < 0) chng = -chng;
        }
        if ((ampchange[cur] + amplist[cur] + i_y + iychange[cur] + chng < max_h)
            && (ampchange[cur] + amplist[cur] + i_y + iychange[cur] + chng >= 50) && pretheta != 0) {
            ampchange[cur] += chng;
        }
        double curf = (prex * 1.0 / mx) * (freqlist[cur] + freqchange[cur]);
        if (curf <= 20 && curf >= 0.1) freqchange[cur] = curf - freqlist[cur];
    }
    prex = mx;
    prey = my;
}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (my >= max_h) {
            int i;
            for (i = 0; i < max_size; i++) {
                double recx1 = 20 + i * 30, recx2 = 40 + i * 30, recy1 = max_h + 50, recy2 = max_h + 70;
                if (mx >= recx1 && mx <= recx2 && my >= recy1 && my <= recy2) {
                    selected[i] = !selected[i];
                    if (selected[i]) visible[i] = 1;
                }
            }
        }
        double c_x = mx;
        got = -1;
        int cur;
        for (cur = 0; cur < max_size - 1; cur++) {
            if (!selected[cur]) continue;
            double A = (amplist[cur] + ampchange[cur]),
            w = 2.0 * PI * (freqlist[cur] + freqchange[cur]) / 400.0,
            phi = (pdiflist[cur] + pdifchange[cur]) * PI / 180.0;
            double x = c_x, y = A * sin(w * x + phi);
            if (cur == 1) y = A * cos (w * x + phi);
            y += (i_y + iychange[cur]);
            if (abs(my - (int) y) < 10) {
                got = cur;
                prex = x;
                prey = my;
                break;
            }
        }
    }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key) {
    int i;
    if(key == 'q')
    {
        exit(0);
    }
    if (key == 'p') {
        ispaused = !ispaused;
    }
    if (ispaused) iPauseTimer(0);
    else iResumeTimer(0);
    if (key == '+') {
        for (i = 0; i < max_size; i++) if (selected[i] && speed[i] * 1.1 <= maxspeed) speed[i] *= 1.1;
    }
    if (key == '-') {
        for (i = 0; i < max_size; i++) if (selected[i] && speed[i] / 1.1 >= 0.01) speed[i] /= 1.1;
    }
    if (key == 'Y') {
        for (i = 0; i < max_size; i++) if (selected[i] && (iychange[i] + i_y + 10) < max_h) iychange[i] += 10;
    }
    if (key == 'y') {
        for (i = 0; i < max_size; i++) if (selected[i] && (iychange[i] + i_y - 10) > 0) iychange[i] -= 10;
    }
    if (key == 'v') {
        showlines = !showlines;
    }
    if (key == 'c') {
        for (i = 0; i < max_size; i++) {
            speed[i] = 1;
            circlepos[i] = 0;
            iychange[i] = 0;
            freqchange[i] = 0;
            iychange[i] = 0;
            ampchange[i] = 0;
            pdifchange[i] = 0;
        }
    }
    if (key == 'A') {
        for (i = 0; i < max_size - 1; i++) {
            if (!selected[i]) continue;
            if (ampchange[i] + amplist[i] + i_y + iychange[i] + 2 < max_h) {
                ampchange[i] += 2;
            }
        }
    }
    if (key == 'a') {
        for (i = 0; i < max_size - 1; i++) {
            if (!selected[i]) continue;
            if (ampchange[i] + amplist[i] - 2 >= 0) {
                ampchange[i] -= 2;
            }
        }
    }
    if (key == 'F') {
        int i;
        for (i = 0; i < max_size - 1; i++) {
            if (!selected[i]) continue;
            if (freqchange[i] + freqlist[i] + .01 < 20) {
                freqchange[i] += .01;
            }
        }
    }
    if (key == 'f') {
        for (i = 0; i < max_size - 1; i++) {
            if (!selected[i]) continue;
            if (freqchange[i] + freqlist[i] - .01 > 0.1) {
                freqchange[i] -= .01;
            }
        }
    }
    if (key == 'X') {
        for (i = 0; i < max_size - 1; i++) {
            if (!selected[i]) continue;
            if (pdifchange[i] + 5 > 360) pdifchange[i] -= 355;
            else pdifchange[i] += 5;
        }
    }
    if (key == 'x') {
        for (i = 0; i < max_size - 1; i++) {
            if (!selected[i]) continue;
            if (pdifchange[i] - 5 < -360) pdifchange[i] += 355;
            else pdifchange[i] -= 5;
        }
    }

    for (i = 0; i < max_size - 1; i++) {
        if (key == (int) (i + '0') && selected[i]) visible[i] = !visible[i];
    }
    if (key == 's') visible[max_size - 1] = !visible[max_size - 1];
    if (key == 'h') help = !help;
    if (key == 'r') {
        int i;
        for (int i = 0; i < max_size; i++) {
            selected[i] = 0;
            visible[i] = 1;
            circlepos[i] = 0;
            speed[i] = 1;
            circlepos[i] = 0;
            iychange[i] = 0;
            freqchange[i] = 0;
            ampchange[i] = 0;
            pdifchange[i] = 0;
            dir[i] = 0;
            showlines = 1;
        }
    }
    //place your codes for other keys here
}

/*
	function iSpecialKeyboard() is called whenver user hits special keys like-
	function keys, home, end, pg up, pg down, arraows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key)
{

    if(key == GLUT_KEY_END)
    {
        exit(0);
    }
    //place your codes for other keys here
}


int main()
{
    //place your own initialization codes here.
    int i;
    for (i = 0; i < max_size; i++){
        colors[i] = i;
        speed[i] = 1;
        visible[i] = 1;
        if (i < 4) {
            selected[i] = 1;
        }
        int num = i + 1;
        stext[i][0] = '\0';
        while (num) {
            char c = '0' + (num % 10);
            strncat(stext[i], &c, 1);
            num /= 10;
        }
        sreverse(i);
    }
    selected[max_size - 1] = 1;
    visible[max_size - 1] = 1;
    iSetTimer(20, circlechange);
    iInitialize(max_w, max_h + 100, "Project Curves");
    return 0;
}
