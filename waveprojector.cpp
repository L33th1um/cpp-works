#include <iostream>
#include <cmath>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <string>
#include <tuple>

using namespace std;

int width, height;
float aspect;
char* screen = nullptr;
const float pixelasp = 11.0f / 24.0f;

mutex screenMutex;

void monitorConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int prevWidth = 0, prevHeight = 0;

    while (true) {
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            int w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            if (w != prevWidth || h != prevHeight) {
                lock_guard<mutex> lock(screenMutex);

                prevWidth = w;
                prevHeight = h;
                width = w;
                height = h;

                if (screen) delete[] screen;
                screen = new char[width * height + 1];
                screen[width * height] = '\0';
                aspect = (float)width / height;
            }
        }
        Sleep(100);
    }
}

tuple<float,float,float,float> edit(float amp, float frec, float prec, float inc) {
			while(true) {
				system("cls");
	        	cout << "Editing wave \n Select option: \n 0 (or anything else) - exit \n (A)mplitude... " << amp << "\n (F)recuency... " << frec << " \n (X)increment... " << inc << " \n (P)recision... " << prec << "\n";
	        	string input;
	        	cin>>input;
	        	switch(input[0]) {
	        		case 'A': {
	        			cout << "Set value for " << input << ": ";
	        			cin>>amp;
	        			//edit(amp,frec,prec,inc);
						break;
					}
					case 'F': {
	        			cout << "Set value for " << input << ": ";
	        			cin>>frec;
	        			//edit(amp,frec,prec,inc);
						break;
					}
					case 'X': {
	        			cout << "Set value for " << input << ": ";
	        			cin>>inc;
	        			//edit(amp,frec,prec,inc);
						break;
					}
					case 'P': {
	        			cout << "Set value for " << input << ": ";
	        			cin>>prec;
	        			//edit(amp,frec,prec,inc);
						break;
					}
					default: {
						return make_tuple(amp,frec,prec,inc);
						
					}
				}
			}
}

int main() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        screen = new char[width * height + 1];
        screen[width * height] = '\0';
        aspect = (float)width / height;
    }
	
	float frec = 3;
    float amp = 0.5;
    float inc = 0.34;
    float prec = 0.05f;
	
	bool editing = false;
	
    thread watcher(monitorConsoleSize);

    for (int t = 0; t < 100000; t++) {
    	
    	
    	
        screenMutex.lock();
        int w = width;
        int h = height;
        float asp = aspect;
        
        if (!screen) {
            screenMutex.unlock();
            continue;
        }

        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                float x = (float)i / w * 2.0f - 1.0f;
                float y = (float)j / h * 2.0f - 1.0f;
                x *= asp * pixelasp;
                char pixel = ' ';
                x += t*inc;
                auto func = sin(frec * x) * amp;
                if (y >= func && y <= func + prec) pixel = '@';
                screen[i + j * w] = pixel;
            }
        }
        screen[w * h] = '\0';
        screenMutex.unlock();
		
		auto drawText = [&](int x, int y, string text) {
			for(int c = 0; c < text.length(); c++) {
				screen[x + y * w + c] = text[c];
			}
		};
		
		for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 8; j++) {
                if(i == 29 || j == 7) {
					screen[i + j * w] = '=';
				}
				else {
					screen[i + j * w] = ' ';
				}
            }
        }
        drawText(0,0,"Wave stats:");
		drawText(0,1,"Amplitude:" + to_string(amp));
		drawText(0,2,"Frequency:" + to_string(frec));
		drawText(0,3,"X increment:" + to_string(inc));
		drawText(0,4,"Char. precision:" + to_string(prec));
		drawText(0,6,"Press ENTER to edit...");
		
		
		if(GetKeyState(VK_RETURN) & 0x8000) {
			if(!editing) {
				editing = true;
				tie(amp,frec,prec,inc) = edit(amp,frec,prec,inc);
				
				editing = false;
			}
		}
		
		
        //printf("%s", screen);
        system("cls");
    }

    watcher.join();
    return 0;
}

