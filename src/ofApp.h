/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

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
*/

	#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include <queue>
#include <stack>
#include <vector>
#include <time.h>
#include <fstream>
#define LINEW 2
#define LINEH 20

class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		bool readFile();
		void freeMemory();
		bool DFS(int x,int y);
		void dfsdraw();
		int HEIGHT;//미로의 높이
		int WIDTH;//미로의 너비
		char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
		int** maze_arr; // 0: 미방문 1: 벽 2: 올바른길 3: 에러
		int** visited;
		int maze_col;//미로칸의 열의 인덱스를 가리킨다.
		int maze_row;//미로칸의 행의 인덱스를 가리킨다.
		int k;
		bool isbfs;
		bool isdfs;
		int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
		
		ofxWinMenu * menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function

		// Used by example app
		ofTrueTypeFont myFont;
        ofImage myImage;
		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

		// Example menu variables
		bool bShowInfo;
		bool bFullscreen;
		bool bTopmost;
		// Example functions
 		void doFullScreen(bool bFull);
		void doTopmost(bool bTop);
		
		//3주차 실습
		int dx[4] = { 1,-1,0,0 };
		int dy[4] = { 0,0,1,-1 };

		//3주차 과제
		queue<pair<int, int>> q_bfs;

		//void BFS(void);
		void bfsdraw(void);
		void init(void);
		// 프로젝트
		//dfs
		void draw_DFS_PATH(void); //DFS 의 경로를 단계적으로 그려준다.
		vector<pair<int, int>> path; // DFS 경로에 해당하는 점을 저장한다.
		int dfs_print_idx = 0; // DFS경로를 출력하기위해 필요한 함수이다.
		bool dfs_draw_finish = false; //DFS경로를 모두 출력했는지 
		//bfs
		bool bfs_draw_finish = false; // BFS경로를 모두 출력했는지

		//generate
		void gen_maze(void); // maze 를 새로 만들기 위한 사전 작업
		void g_maze(int x, int y); // maze를 만들때 사용되는 백트래킹 함수
		int** new_maze; //새로 만든 maze를 저장하기 위한 포인터
};
