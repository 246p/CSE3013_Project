/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#pragma warning(disable:6011)
#define _CRT_SECURE_NO_WARNINGS_
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Project made by KMJ"); // Set the app name on the title bar
	ofSetFrameRate(30);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isbfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	menu->AddPopupItem(hPopup, "Generate", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost

	//menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();
	// �����Լ��� �����ϱ� ���ؼ�  �ð��� ������ ����ǥ�� �ʱ�ȭ���ش�.
	srand((unsigned)time(NULL)); 
} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		readFile();
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	if (title == "Generate") {
		gen_maze();
	}
	//
	// Window menu
	//
	if (title == "Show DFS") {
		bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)//DFS �� �ǽ��Ѵ�. 
		{
			isbfs = false;
			dfs_draw_finish = false;
			init(); // ������ ���α׷��� ������ ������� �ʱ�ȭ�����ش�.
			DFS(1, 1);
			bShowInfo = true;
			isdfs = true;
		}
		else
			cout << "you must open file first" << endl;
	}

	if (title == "Show BFS") {
		//doTopmost(bChecked); // Use the checked value directly
		if (isOpen)//BFS �� �����Ѵ�. 
		{
			isdfs = false;
			init();
			q_bfs.push(make_pair(1, 1));
			maze_arr[1][1] = 1;
			bShowInfo = true;
			isbfs = true;
		}
		else
			cout << "you must open file first" << endl;
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {
	if (isdfs && !dfs_draw_finish) { 
		dfs_print_idx++; // DFS �� ��θ� �Ѵܰ辿 �� �׷��ش�.
	}
	if (isbfs && !bfs_draw_finish)  //BFS�� �ܰ�� DFS�ʹ� �ٸ��� update�Լ��� ���� �����Ѵ�.
		if (!q_bfs.empty()) {
			pair<int, int> curr = q_bfs.front(); q_bfs.pop();
			int x = curr.first, y = curr.second;
			if (x == WIDTH - 2 && y == HEIGHT - 2) {
				while (!q_bfs.empty()) q_bfs.pop(); //BFS�� ����ȴٸ� queue�� �ִ� ��� ���Ҹ� ���ش�.
				bfs_draw_finish = true;
			}
			else 
				for (int i = 0; i < 4; i++) {
					if (y + 2 * dy[i] < 0 || x + 2 * dx[i] < 0) continue;
					if (maze_arr[y + dy[i]][x + dx[i]] == 1) continue;
					if (maze_arr[y + 2 * dy[i]][x + 2 * dx[i]] != 0) continue;
					maze_arr[y + 2 * dy[i]][x + 2 * dx[i]] = maze_arr[y][x] + 1;
					q_bfs.push(make_pair(x + 2 * dx[i], y + 2 * dy[i]));
				}
		}
}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	// TO DO : DRAW MAZE; 
	// ����� �ڷᱸ���� �̿��� �̷θ� �׸���.
	// add code here
	int w = LINEW, h = LINEH; // �̷θ� �׸��� ���� ���̿� �β��� �ǹ��Ѵ�.
	if (isOpen) { // �̷��� ������ �׷��ش�.
		int x = 0, y = 0;
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				if (input[i][j] == '|')
				{
					ofDrawRectangle(x, y, w, h);
					x += w;
				}
				else if (input[i][j] == '-') {
					ofDrawRectangle(x, y, h, w);
					x += h;
				}
				else if (input[i][j] == ' ') {
					if (j % 2 == 0)
						x += w;
					else
						x += h;
				}
				else if (input[i][j] == '+') {
					ofDrawRectangle(x, y, w, w);
					x += w;
				}
			}
			x = 0;
			if (i % 2 == 0)
				y += w;
			else
				y += h;
		}
	}

	if (isbfs) {
		ofSetColor(200); ofSetLineWidth(5);
		if (isOpen) {
			bfsdraw(); // ������� bfs��θ� �׷��ش�.
			if (!bfs_draw_finish) {
				// ���� Ž������ ���̿� �ִ� ��� ���� ǥ���ϱ� ���� 
				// ù ����(���� �����̴� ����)�� �ʷϻ����� �׷��ش�
				pair<int, int> temp = q_bfs.front(); q_bfs.pop();
				ofSetColor(0, 255, 0);
				ofDrawCircle(temp.first * (LINEH + LINEW) / 2 + (LINEW / 2), temp.second * (LINEH + LINEW) / 2 + (LINEW / 2), LINEH / 4);
				q_bfs.push(temp);
				//������ ���ҵ��� �ϳ��� ���� ȸ������ �׷��ְ� �ٽ� ť�� �־��ش�.
				for (int i = 1;i < q_bfs.size();i++) {
					temp = q_bfs.front(); q_bfs.pop();
					ofSetColor(100);
					ofDrawCircle(temp.first * (LINEH + LINEW) / 2 + (LINEW / 2), temp.second * (LINEH + LINEW) / 2 + (LINEW / 2), LINEH/4);
					q_bfs.push(temp);
				}
				ofSetColor(100);
			}
		}
		else
			cout << "You must open file first" << endl;
	}
	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) {
			if (!dfs_draw_finish)  //dfs�� �Ϸ���� �ʾҴٸ� �߰� ���� ���� �׷��ش�.
				draw_DFS_PATH();
			else
				dfsdraw(); // dfs�� �Ϸ�Ǿ��ٸ� ��ü�� �׷��ش�.
		}
		else
			cout << "You must open file first" << endl;
	}
	if (bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project 20201559");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			freeMemory();
			ofExit();
		}
	}
	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		//menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;


			// TO DO
			// .maz ������ input���� �޾Ƽ� ������ �ڷᱸ���� �ִ´�
			freeMemory();
			isbfs = isdfs = false;
			HEIGHT = 0;
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; it++) {
				string line = *it;
				WIDTH = line.size();
				HEIGHT++;
			}
			input = (char**)malloc(sizeof(char*) * HEIGHT);
			maze_arr = (int**)malloc(sizeof(int*) * (HEIGHT));
			for (int i = 0;i < HEIGHT;i++) {
				input[i] = (char*)malloc(sizeof(char) * WIDTH);
				maze_arr[i] = (int*)malloc(sizeof(int) * (WIDTH));
			}

			maze_row = 0;
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; it++) {
				string line = *it;
				for (maze_col = 0;maze_col < WIDTH;maze_col++)
					input[maze_row][maze_col] = line[maze_col];
				maze_row++;
			}
			init();

			isOpen = 1;
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() { 
	//TO DO
	// malloc�� memory�� free���ִ� �Լ�
	for (int i = 0;i < HEIGHT;i++) {
		free(input[i]);
		free(maze_arr[i]);
	}
	free(maze_arr);
	free(input);
}
bool ofApp::DFS(int x, int y)//DFSŽ���� �ϴ� �Լ�
{
	//path ������ dfs���� �̵��� ������ ��ġ�� �����ϴ� �����̴�.
	path.push_back(make_pair(x, y)); 
	if (x == WIDTH - 2 && y == HEIGHT - 2) {
		maze_arr[y][x] = 2;
		return true;
	}
	else 
		for (int i = 0;i < 4;i++) 
			if (x + dx[i] * 2 > 0 && x + dx[i] * 2 < WIDTH && y + dy[i] * 2 > 0 && y + dy[i] * 2 < HEIGHT && maze_arr[y + dy[i]][x + dx[i]] == 0) 
				if (maze_arr[y + 2 * dy[i]][x + 2 * dx[i]] == 0) {
					maze_arr[y][x] = 3;;
					maze_arr[y + 2 * dy[i]][x + 2 * dx[i]] = 3;
					if (DFS(x + 2 * dx[i], y + 2 * dy[i])) {
						maze_arr[y][x] = 2;
						return true;
					}
					path.push_back(make_pair(x, y));
					maze_arr[y][x] = 3;
				}
	maze_arr[y][x] = 3;
	return false;
}
void ofApp::draw_DFS_PATH(void) { // path�� ����� ���� �������� �׷��ش�.
	if (dfs_print_idx >= path.size() - 1) {
		dfs_draw_finish = true;
		return;
	}
	for (int i = 0;i < dfs_print_idx;i++) { //path�� ���� �̾ �׷��ش�.
		int x1 = path[i].first;
		int y1 = path[i].second;
		int x2 = path[i + 1].first;
		int y2 = path[i + 1].second;
		ofSetLineWidth(5);
		ofSetColor(200);
		ofDrawLine(x1 * (LINEH + LINEW) / 2 + (LINEW / 2), y1 * (LINEH + LINEW) / 2 + (LINEW / 2), x2 * (LINEH + LINEW) / 2 + (LINEW / 2), y2 * (LINEH + LINEW) / 2 + (LINEW / 2));
	}
	ofSetColor(100);
	ofDrawCircle(path[dfs_print_idx].first * (LINEH + LINEW) / 2 + (LINEW / 2), path[dfs_print_idx].second*(LINEH + LINEW) / 2 + (LINEW / 2), LINEH/4);
}

void ofApp::dfsdraw() // DFS�� �Ϸ�Ǿ��ٸ� maze_arr�� ������ ������� ���� �׷��ش�.
{
	for (int i = 1;i < HEIGHT;i += 2) {
		for (int j = 1;j < WIDTH;j += 2) {
			for (int k = 0;k < 4;k++) {
				if (i + 2 * dy[k] < 0 || j + 2 * dx[k] < 0)
					continue;
				if (maze_arr[i + dy[k]][j + dx[k]] == 1) continue;
				if (maze_arr[i][j] == 2 && maze_arr[i + 2 * dy[k]][j + 2 * dx[k]] == 2) {
					ofSetColor(100);
					ofDrawLine(j * (LINEH + LINEW) / 2 + (LINEW / 2), i *(LINEH + LINEW) / 2 + (LINEW / 2), (j + 2 * dx[k]) * (LINEH + LINEW) / 2 + (LINEW / 2), (i + 2 * dy[k]) * (LINEH + LINEW) / 2 + (LINEW / 2));
				}
				else if (maze_arr[i][j] == 2 && maze_arr[i + 2 * dy[k]][j + 2 * dx[k]] == 3) {
					ofSetColor(200);
					ofDrawLine(j * (LINEH + LINEW) / 2 + (LINEW / 2), i * (LINEH + LINEW) / 2 + (LINEW / 2), (j + 2 * dx[k]) * (LINEH + LINEW) / 2 + (LINEW / 2), (i + 2 * dy[k]) * (LINEH + LINEW) / 2 + (LINEW / 2));
				}
				else if (maze_arr[i][j] == 3 && maze_arr[i + 2 * dy[k]][j + 2 * dx[k]] == 2) {
					ofSetColor(200);
					ofDrawLine(j * (LINEH + LINEW) / 2 + (LINEW / 2), i * (LINEH + LINEW) / 2 + (LINEW / 2), (j + 2 * dx[k]) * (LINEH + LINEW) / 2 + (LINEW / 2), (i + 2 * dy[k]) * (LINEH + LINEW) / 2 + (LINEW / 2));
				}
				else if (maze_arr[i][j] == 3 && maze_arr[i + 2 * dy[k]][j + 2 * dx[k]] == 3) {
					ofSetColor(200);
					ofDrawLine(j * (LINEH + LINEW) / 2 + (LINEW / 2), i * (LINEH + LINEW) / 2 + (LINEW / 2), (j + 2 * dx[k]) * (LINEH + LINEW) / 2 + (LINEW / 2), (i + 2 * dy[k]) * (LINEH + LINEW) / 2 + (LINEW / 2));
				}
			}
		}
	}
}

void ofApp::bfsdraw(void) { //dfs������ ����� �� ���� �׷��ش�.
	for (int i = 1;i < HEIGHT;i += 2) {
		for (int j = 1;j < WIDTH;j += 2) {
			for (int k = 0;k < 4;k++) {
				if (i + 2 * dy[k] < 0 || j + 2 * dx[k] < 0)
					continue;
				if (maze_arr[i + dy[k]][j + dx[k]] == 1) continue;
				if (maze_arr[i][j] != 0 && maze_arr[i + 2 * dy[k]][j + 2 * dx[k]] != 0) {
					ofSetColor(200);
					ofDrawLine(j * (LINEH + LINEW) / 2 + (LINEW / 2), i * (LINEH + LINEW) / 2 + (LINEW / 2), (j + 2 * dx[k]) * (LINEH + LINEW) / 2 + (LINEW / 2), (i + 2 * dy[k]) * (LINEH + LINEW) / 2 + (LINEW / 2));
				}
			}
		}
	}
	int x = WIDTH - 2, y = HEIGHT - 2;
	int depth = maze_arr[y][x];
	if (maze_arr[y][x] != 0) {
		while (depth != 1) {
			for (int k = 0;k < 4;k++) {
				if (maze_arr[y + dy[k]][x + dx[k]] == 1) continue;
				if (maze_arr[y + 2 * dy[k]][x + 2 * dx[k]] == depth - 1) {
					ofSetColor(100);
					ofDrawLine(x * (LINEH + LINEW) / 2 + (LINEW / 2), y * (LINEH + LINEW) / 2 + (LINEW / 2), (x + 2 * dx[k]) * (LINEH + LINEW) / 2 + (LINEW / 2), (y + 2 * dy[k]) * (LINEH + LINEW) / 2 + (LINEW / 2));
					x = x + 2 * dx[k];
					y = y + 2 * dy[k];
					depth = maze_arr[y][x];
					break;
				}
			}
		}
	}
}
void ofApp::gen_maze(void) { 
	// maze�� �������ش�. �ǽ� �ð����� �ٸ��� ��Ʈ��ŷ�� ���Ͽ� �����Ͽ���.
	// ���� �̹� ������ �����ִٸ� �޸𸮸� �ʱ�ȭ���ش�.
	if (isOpen) freeMemory();
	isbfs = isdfs = isOpen = false;
	int w, h;//w,h�� ���� ��
	printf("input W,H : "); scanf("%d %d", &w, &h);
	//�Է¹��� ũ�⿡ �°� ���� �Ҵ� ���ش�.
	WIDTH = 2 * w + 1; HEIGHT = 2 * h + 1;
	new_maze = (int**)malloc(sizeof(int*) * (2 * h + 1));
	for (int i = 0;i < (2 * h + 1);i++)
		new_maze[i] = (int*)malloc(sizeof(int) * (2 * w + 1));
	for (int i = 0;i < 2 * h + 1;i++)
		for (int j = 0;j < 2 * w + 1;j++)
			new_maze[i][j] = 1;
	//������ġ�� ��ǥ�� �����Լ��� ���� �������ش�.
	int sx = (rand() % w) * 2 + 1; int sy = (rand() % h) * 2 + 1;
	new_maze[sy][sx] = 0;
	g_maze(sx, sy); // g_maze��� �Լ��� ���� ��Ʈ��ŷ�� �̿��� �̷� ������ �ǽ��Ѵ�.

	//���� ���� �����ϱ����ؼ� input �� maze_arr �迭�� �����Ҵ� ���ش�.
	input = (char**)malloc(sizeof(char*) * HEIGHT);
	maze_arr = (int**)malloc(sizeof(int*) * (HEIGHT));
	for (int i = 0;i < HEIGHT;i++) {
		input[i] = (char*)malloc(sizeof(char) * WIDTH);
		maze_arr[i] = (int*)malloc(sizeof(int) * (WIDTH));
	}
	// ������ �̷θ� input���Ͽ� �����Ѵ�.
	for (int i = 0;i < HEIGHT;i++) {
		for (int j = 0;j < WIDTH;j++) {
			if (i % 2 == 0) {
				if (j % 2 == 0)
					input[i][j] = '+';
				else {
					if (new_maze[i][j] == 1)
						input[i][j] = '-';
					else
						input[i][j] = ' ';
				}
			}
			else {
				if (new_maze[i][j] == 1)
					input[i][j] = '|';
				else
					input[i][j] = ' ';

			}
		}
	}
	init();
	isOpen = 1;

	//������ �����Ѵ�.
	//ofFile�� ����Ѵ��� �⺻ ���ڵ��� UTF-16���� �Ǳ⿡ C����� FILE �� ����Ѵ�.
	char temp[20]; // �Է¹޴� ������ ������ �̸�
	char filename[30] = { "data/" }; // ������ ����θ� ���Ͽ� ������ġ�� �������ش�.
	printf("FILE NAME : ");
	scanf("%s", temp);
	strcat(filename, temp);
	strcat(filename, ".maz"); // Ȯ���ڴ� �ڵ����� .maz�� ���߾��ش�.
	FILE* fp = fopen(filename, "w");
	for (int i = 0;i < HEIGHT;i++) {
		for (int j = 0;j < WIDTH;j++)
			fprintf(fp, "%c", input[i][j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
	// new_maze �� ����� �Ϸ�Ǿ����Ƿ� �޸� �Ҵ��� �������ش�.
	for (int i = 0;i < 2 * h + 1;i++)
		free(new_maze[i]);
	free(new_maze);
}

void ofApp::g_maze(int x, int y) {
	int d = rand() % 4; //������ ������ �������ְ� �� ������ ���� �㹰���ش�.
	for (int i = 0;i < 4;i++) {
		int tx = x + dx[(i + d) % 4] * 2;
		int ty = y + dy[(i + d) % 4] * 2;
		if (tx > 0 && tx < WIDTH && ty>0 && ty < HEIGHT && new_maze[ty][tx] == 1) {
			new_maze[ty][tx] = 0;
			new_maze[y + dy[(i + d) % 4]][x + dx[(i + d) % 4]] = 0;
			g_maze(tx, ty); // ��������� �̸� �ݺ��Ѵ�.
		}
	}
}

void ofApp::init(void) { // ���� ������ ���� �����ִ� ������ �Ѵ�.
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (input[i][j] != ' ')
				maze_arr[i][j] = 1;
			else
				maze_arr[i][j] = 0;
		}
	}
	path.clear();
	dfs_draw_finish = false;
	dfs_print_idx = 0;
	bfs_draw_finish = false;
}