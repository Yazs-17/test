#include <graphics.h>
#include <string>
#include <iostream>
#include <vector>
#include <FhStatus.h>
#include <easyx.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
#pragma comment(lib, "winmm.lib") // 链接winmm.lib库,media player
#pragma comment(lib, "MSIMG32.LIB") // 链接MSIMG32.LIB库
inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

class Anim {
public:
	Anim(LPCTSTR path, int num, int internal) {
		interval_ms = internal;

		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);


			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	~Anim() { // 析构
		for (size_t i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}
	}

	void play(int x, int y, int delta) {
		timer += delta;
		if (timer >= interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, frame_list[idx_frame]);

	}
private:
	int timer = 0;
	int idx_frame = 0;

	std::vector<IMAGE*>frame_list;
	int interval_ms = 0;
};
class Player {
public:
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
public:
	Player() {
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Anim(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Anim(_T("img/player_right_%d.png"), 6, 45);
	}
	~Player() {
		delete anim_left;
		delete anim_right;
	}
	void ProcessEvent(const ExMessage& msg) {
		if (msg.message == WM_KEYDOWN) {
			switch (msg.vkcode) {
			case VK_UP:
				is_m_u = true;
				break;
			case VK_DOWN:
				is_m_d = true;
				break;
			case VK_LEFT:
				is_m_l = true;
				break;
			case VK_RIGHT:
				is_m_r = true;
				break;
			}
		}
		else if (msg.message == WM_KEYUP) {
			switch (msg.vkcode) {
			case VK_UP:
				is_m_u = false;
				break;
			case VK_DOWN:
				is_m_d = false;
				break;
			case VK_LEFT:
				is_m_l = false;
				break;
			case VK_RIGHT:
				is_m_r = false;
				break;
			}
		}
	}
	void Move() {
		int dir_x = is_m_r - is_m_l;
		int dir_y = is_m_d - is_m_u;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double n_x = dir_x / len_dir;
			double n_y = dir_y / len_dir;
			p_pos.x += (int)(SPEED * n_x);
			p_pos.y += (int)(SPEED * n_y);
		}
		if (p_pos.x < 0)p_pos.x = 0;
		if (p_pos.y < 0)p_pos.y = 0;
		if (p_pos.x + FRAME_WIDTH > WINDOW_WIDTH)p_pos.x = WINDOW_WIDTH - FRAME_WIDTH;
		if (p_pos.y + FRAME_HEIGHT > WINDOW_HEIGHT)p_pos.y = WINDOW_HEIGHT - FRAME_HEIGHT;
	}
	void Draw(int delta) {
		int pos_shadow_x = p_pos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = p_pos.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		static bool facing_l = false;
		int dir_x = is_m_r - is_m_l;
		if (dir_x > 0)facing_l = false;
		else if (dir_x < 0) facing_l = true;


		if (facing_l)anim_left->play(p_pos.x, p_pos.y, delta);
		else anim_right->play(p_pos.x, p_pos.y, delta);

	}
	POINT GetPosition()const {
		return p_pos;
	}
private:
	const int SPEED = 3;
	const int SHADOW_WIDTH = 32;
private:
	IMAGE img_shadow;
	Anim* anim_left;
	Anim* anim_right;
	POINT p_pos = { 500,500 };
	bool is_m_u = false;
	bool is_m_d = false;
	bool is_m_l = false;
	bool is_m_r = false;
};
class Bullet {
public:
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;
	void Draw()const {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);

	}
private:
	const int RADIUS = 10;
};
class Enemy {
public:
	Enemy() {
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Anim(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Anim(_T("img/enemy_right_%d.png"), 6, 45);

		// 敌人生成边界
		enum class SpawnEdge {
			Up = 0,
			Down,
			Left,
			Right
		};

		// 随机出现
		SpawnEdge edge = (SpawnEdge)(rand() % 4);// 0-3
		switch (edge) {
			// rand() 的范围是 0 - 32767
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;// 0-1279
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
		}

	}

	bool CheckBulletCollision(const Bullet& bullet) {
		// 将子弹等效为点，判断点是否在敌人矩形内
		bool is_x_in = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_y_in = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;
		return is_x_in && is_y_in;
	}

	bool CheckPlayerCollision(const Player& player) {

		// 将敌人中心位置等效为点，判断点是否在玩家矩形内
		POINT check_position = { position.x + FRAME_WIDTH / 2, position.y + FRAME_HEIGHT / 2 };
		POINT player_position = player.GetPosition();
		bool is_x_in = check_position.x >= player_position.x && check_position.x <= player_position.x + player.FRAME_WIDTH;
		bool is_y_in = check_position.y >= player_position.y && check_position.y <= player_position.y + player.FRAME_HEIGHT;
		return is_x_in && is_y_in;

	}

	void Move(const Player& player) {
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double n_x = dir_x / len_dir;
			double n_y = dir_y / len_dir;
			position.x += (int)(SPEED * n_x);
			position.y += (int)(SPEED * n_y);
		}
		if (dir_x > 0)facing_left = false;
		else if (dir_x < 0)facing_left = true;
	}

	void Draw(int delta) {
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left)anim_left->play(position.x, position.y, delta);
		else anim_right->play(position.x, position.y, delta);
	}

	~Enemy() {
		delete anim_left;
		delete anim_right;
	}
	void Hurt() {
		alive = false;
	}

	bool CheckAlive() {
		return alive;
		if (!alive) {
			delete this;
		}
	}
private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
	IMAGE img_shadow;
	Anim* anim_left;
	Anim* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;

};

// 2024 - 8 - 18 ADDED

class Button {
public:
	Button(RECT rect, LPCSTR path_img_idle, LPCSTR path_img_hovered, LPCSTR path_img_pushed) {
		region = rect;
		//loadimage(&img_idle, path_img_idle);
		//loadimage(&img_hovered, path_img_hovered);
		//loadimage(&img_pushed, path_img_pushed);
	}
	~Button() {
	}
	void Draw() {
		switch (status) {
		case Status::Idle:
			putimage(region.left,region.top,&img_idle);
			break;

		case Status::Hovered:
			putimage(region.left,region.top,&img_hovered);		
			break;

		case Status::Pushed:
			putimage(region.left,region.top,&img_pushed);
			break;
		}


	}
private:
	enum class Status {
		Idle = 0,
		Hovered,
		Pushed
	};
private:
	RECT  region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;

};



void TryGenerateEnemy(std::vector<Enemy*>& enemy_list) {
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}

void UpdateBullets(std::vector<Bullet>& bullet_list,const Player& player) {
	const double RADIAL_SPEED = 0.0045;                          // 径向波动速度
	const double TANGENT_SPEED = 0.0055;                         // 切向波动速度
	double radian_interval = 2 * 3.1415926 / bullet_list.size(); // 子弹之间的弧度间隔
	POINT player_pos = player.GetPosition();
	double radius = 100 + 25*sin(GetTickCount() * RADIAL_SPEED); // 径向波动半径
	for (size_t i = 0; i < bullet_list.size(); i++) {
		double radian = radian_interval * i + GetTickCount() * TANGENT_SPEED;
		bullet_list[i].position.x = player_pos.x + player.FRAME_WIDTH / 2 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_pos.y + player.FRAME_HEIGHT / 2 + (int)(radius * cos(radian));
	}
}

void DrawPlaterScore(int score) {
	static TCHAR text[64];
	_stprintf_s(text, _T("当前玩家得分：%d"), score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);

}

int main2() {


	std::cout << "Hello World!" << std::endl;

	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);

	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);

	bool running = true;

	int score = 0;
	Player player;
	ExMessage msg;
	IMAGE img_background;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);

	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running) {
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg) ){
			if (msg.message == WM_KEYDOWN) {
				if (msg.vkcode == VK_ESCAPE) {
					running = false;
					break;
				}
			}
			player.ProcessEvent(msg);
		}
		player.Move();
		UpdateBullets(bullet_list, player);
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list) {
			enemy->Move(player);
		}
		// 检测玩家与敌人的碰撞
		for (Enemy* enemy: enemy_list) {
			if (enemy->CheckPlayerCollision(player)) {
				//MessageBox(GetHWnd(), _T("扣“1”观看战败CG"), _T("Game Over"), MB_OK);
				static TCHAR text[128];
				_stprintf_s(text, _T("最终得分：%d！"), score);
				MessageBox(GetHWnd(), text, _T("Game Over"), MB_OK);
				running = false;
				break;
			}
		}

		// 检测子弹与敌人的碰撞
		for (Bullet& bullet : bullet_list) {
			for (Enemy* enemy : enemy_list) {
				if (enemy->CheckBulletCollision(bullet)) {
					mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					enemy->Hurt();
					score++;
				}
			}
		}
		// 清理已经死亡的敌人
		for (size_t i = 0; i < enemy_list.size(); i++) {
			if (!enemy_list[i]->CheckAlive()) {
				delete enemy_list[i];
				enemy_list.erase(enemy_list.begin() + i);  // 也可以先交换到最后，然后再删除最后一个元素，这样可以减少删除的时间复杂度
				//具体做法是：
				// for (size_t i = 0; i < enemy_list.size(); i++) {
				// 	if (!enemy_list[i]->CheckAlive()) {
				// 		delete enemy_list[i];
				// 		enemy_list[i] = enemy_list.back();
				// 		enemy_list.pop_back();
				// 		i--;
				// 	}
				// }
				// 这样可以减少删除的时间复杂度，但是会改变敌人的顺序，如果不需要保持敌人的顺序，可以使用这种方法
				// 如果需要保持敌人的顺序，就只能使用上面的方法，但是时间复杂度较高
				// 也可以使用迭代器，但是迭代器的使用比较复杂，不推荐

				i--; // 删除后，索引需要减一，否则会跳过一个元素，导致内存泄漏，程序崩溃，或者无法删除所有敌人，等等问题
			}
		}

		cleardevice();

		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		for (Enemy* enemy : enemy_list) {
			enemy->Draw(1000 / 144);
		}
		for (const Bullet& bullet : bullet_list) {
			bullet.Draw();
		}
		DrawPlaterScore(score);
		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144) {
			Sleep(1000 / 144 - delta_time);
		}
	}



	return 0;
}