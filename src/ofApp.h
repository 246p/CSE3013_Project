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
		int HEIGHT;//�̷��� ����
		int WIDTH;//�̷��� �ʺ�
		char** input;//�ؽ�Ʈ ������ ��� ������ ��� ������ �迭�̴�.
		int** maze_arr; // 0: �̹湮 1: �� 2: �ùٸ��� 3: ����
		int** visited;
		int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int k;
		bool isbfs;
		bool isdfs;
		int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
		
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
		
		//3���� �ǽ�
		int dx[4] = { 1,-1,0,0 };
		int dy[4] = { 0,0,1,-1 };

		//3���� ����
		queue<pair<int, int>> q_bfs;

		//void BFS(void);
		void bfsdraw(void);
		void init(void);
		// ������Ʈ
		//dfs
		void draw_DFS_PATH(void); //DFS �� ��θ� �ܰ������� �׷��ش�.
		vector<pair<int, int>> path; // DFS ��ο� �ش��ϴ� ���� �����Ѵ�.
		int dfs_print_idx = 0; // DFS��θ� ����ϱ����� �ʿ��� �Լ��̴�.
		bool dfs_draw_finish = false; //DFS��θ� ��� ����ߴ��� 
		//bfs
		bool bfs_draw_finish = false; // BFS��θ� ��� ����ߴ���

		//generate
		void gen_maze(void); // maze �� ���� ����� ���� ���� �۾�
		void g_maze(int x, int y); // maze�� ���鶧 ���Ǵ� ��Ʈ��ŷ �Լ�
		int** new_maze; //���� ���� maze�� �����ϱ� ���� ������
};
