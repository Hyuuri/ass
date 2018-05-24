#include <curses.h>
#include <random>
#include <iostream>


//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor{NONE, RED, BLUE, GREEN, YELLOW};

//ぷよ1つのデータを格納する構造体
struct puyodata
{
	enum puyocolor color;	//色
	bool handling;	//操作中か否か
	bool handling_special;

	//構造体のコンストラクタ
	//この構造体のインスタンス定義時に一度だけ実行される関数
	//ここでは変数の値を初期化する
	puyodata()
	{
		color = NONE;
		handling = false;
		handling_special = false;
	}
};



class Field {
private:
    struct puyodata* field_array;
    unsigned int field_line;
    unsigned int field_column;
    void Release();
public:
    void ChangeSize(unsigned int line, unsigned int column);
    struct puyodata GetValue(unsigned int y, unsigned int x);
    void SetValue(unsigned int y, unsigned int x, struct puyodata data);
    unsigned int GetLine();
    unsigned int GetColumn();
    Field();
    ~Field();
};

Field::Field() {
    struct puyodata *field_array = NULL;
    unsigned int field_line = 0;
    unsigned int field_column = 0;
    ChangeSize(LINES/2,COLS/2);
};
Field::~Field() {
    Release();
};


class FieldControl :
    public Field{
public:
    bool Move;
    bool FallPuyo();
    void GeneratePuyo();
    void LandingPuyo();
    void ControlDown();
    void ControlLeft();
    void ControlRight();
    int  VanishPuyo();
    int  VanishPuyo(unsigned int y, unsigned int x);
    int LineMove(int x);
    FieldControl();
};

FieldControl::FieldControl() {
    GeneratePuyo();
    Move = true;
};

int FieldControl::LineMove(int x){
	for (int y = 0; y <GetLine();y++)
		if (GetValue(y,x).handling == false){
			if (GetValue(y,x).color != NONE){
				struct puyodata temp;
				temp.color = GetValue(y,x).color;
				temp.handling = true;
				temp.handling_special = true;
				SetValue(y,x,temp);
			}	
		}
	return 0;
}
bool FieldControl::FallPuyo(){
	for (int y = 0; y < GetLine(); y++)
                {
                        for (int x = 0; x < GetColumn(); x++)
                        {
                                if (GetValue(y,x).handling == true)
					return false;
                        }
                }
	return true;
}

int FieldControl::VanishPuyo()
	{
		int vanishednumber = 0;
		for (int y = 0; y < GetLine(); y++)
		{
			for (int x = 0; x < GetColumn(); x++)
			{
				vanishednumber += VanishPuyo(y, x);
			}
		}

		return vanishednumber;
	}

	//ぷよ消滅処理を座標(x,y)で行う
	//消滅したぷよの数を返す
int FieldControl::VanishPuyo(unsigned int y, unsigned int x)
	{
		//判定個所にぷよがなければ処理終了
		if (GetValue(y, x).color == NONE)
		{
			return 0;
		}


		//判定状態を表す列挙型
		//NOCHECK判定未実施，CHECKINGが判定対象，CHECKEDが判定済み
		enum checkstate{ NOCHECK, CHECKING, CHECKED };

		//判定結果格納用の配列
		enum checkstate *field_array_check;
		field_array_check = new enum checkstate[GetLine()*GetColumn()];

		//配列初期化
		for (int i = 0; i < GetLine()*GetColumn(); i++)
		{
			field_array_check[i] = NOCHECK;
		}

		//座標(x,y)を判定対象にする
		field_array_check[y*GetColumn() + x] = CHECKING;

		//判定対象が1つもなくなるまで，判定対象の上下左右に同じ色のぷよがあるか確認し，あれば新たな判定対象にする
		bool checkagain = true;
		while (checkagain)
		{
			checkagain = false;

			for (int y = 0; y < GetLine(); y++)
			{
				for (int x = 0; x < GetColumn(); x++)
				{
					//(x,y)に判定対象がある場合
					if (field_array_check[y*GetColumn() + x] == CHECKING)
					{
						//(x+1,y)の判定
						if (x < GetColumn() - 1)
						{
							//(x+1,y)と(x,y)のぷよの色が同じで，(x+1,y)のぷよが判定未実施か確認
							if (GetValue(y, x + 1).color == GetValue(y, x).color && field_array_check[y*GetColumn() + (x + 1)] == NOCHECK)
							{
								//(x+1,y)を判定対象にする
								field_array_check[y*GetColumn() + (x + 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(x-1,y)の判定
						if (x > 0)
						{
							if (GetValue(y, x - 1).color == GetValue(y, x).color && field_array_check[y*GetColumn() + (x - 1)] == NOCHECK)
							{
								field_array_check[y*GetColumn() + (x - 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(x,y+1)の判定
						if (y < GetLine() - 1)
						{
							if (GetValue(y + 1, x).color == GetValue(y, x).color && field_array_check[(y + 1)*GetColumn() + x] == NOCHECK)
							{
								field_array_check[(y + 1)*GetColumn() + x] = CHECKING;
								checkagain = true;
							}
						}

						//(x,y-1)の判定
						if (y > 0)
						{
							if (GetValue(y - 1, x).color == GetValue(y, x).color && field_array_check[(y - 1)*GetColumn() + x] == NOCHECK)
							{
								field_array_check[(y - 1)*GetColumn() + x] = CHECKING;
								checkagain = true;
							}
						}

						//(x,y)を判定済みにする
						field_array_check[y*GetColumn() + x] = CHECKED;
					}
				}
			}
		}

		//判定済みの数をカウント
		int puyocount = 0;
		for (int i = 0; i < GetLine()*GetColumn(); i++)
		{
			if (field_array_check[i] == CHECKED)
			{
				puyocount++;
			}
		}

		//4個以上あれば，判定済み座標のぷよを消す
		int vanishednumber = 0;
		if (4 <= puyocount)
		{
			for (int y = 0; y < GetLine(); y++)
			{
				for (int x = 0; x < GetColumn(); x++)
				{
					if (field_array_check[y*GetColumn() + x] == CHECKED)
					{
						struct puyodata data = GetValue(y, x);
						data.color = NONE;
						data.handling = false;
						SetValue(y, x, data);
						LineMove(x);

						vanishednumber++;
					}
				}
			}
		}

		//メモリ解放
		delete[] field_array_check;

		return vanishednumber;
	}


unsigned int Field::GetLine(){
   return field_line;
}

unsigned int Field::GetColumn(){
    return field_column;
}

//field_arrayのメモリ開放
void Field::Release()
{
	if (field_array == NULL)
	{
		return;
	}

	delete[] field_array;
	field_array = NULL;
}


//フィールドサイズ変更
void Field::ChangeSize(unsigned int line, unsigned int column)
{
	Release();

	//新しいサイズでメモリ確保
	field_array = new struct puyodata[line*column];

	field_line = line;
	field_column = column;
}

//フィールドの指定された位置の値を返す
struct puyodata Field::GetValue(unsigned int y, unsigned int x)
{
	if (y >= field_line || x >= field_column)
	{
		//引数の値が正しくない
		struct puyodata temp;
		return temp;
	}  

	return field_array[y*field_column + x];
}

//フィールドの指定された位置に値を書き込む
void Field::SetValue(unsigned int y, unsigned int x, struct puyodata data)
{
	if (y >= field_line || x >= field_column)
	{
		//引数の値が正しくない
		return;
	}

	field_array[y*field_column + x] = data;
}


//フィールドに新しいぷよ生成
void FieldControl::GeneratePuyo()
{
//操作可能フラグの初期化
    Move = true;

struct puyodata newpuyo[4];

    newpuyo[0].color = RED;
    newpuyo[0].handling = true;

    newpuyo[1].color = BLUE;
    newpuyo[1].handling = true;

    newpuyo[2].color = YELLOW;
    newpuyo[2].handling = true;

    newpuyo[3].color = GREEN;
    newpuyo[3].handling = true;

    //std::random_device rd;

    //SetValue(0, 5, newpuyo[rd()%4]);
    //SetValue(0, 6, newpuyo[rd()%4]);
    SetValue(0, 5, newpuyo[3]);
    SetValue(0, 6, newpuyo[2]);
}

//操作中ぷよの着地判定
//着地判定があるとtrueを返す
void FieldControl::LandingPuyo()
{
	for (int y = 0; y < GetLine() ; y++)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (GetValue(y,x).handling == true)
			{
				if (GetValue(y,x).handling_special == true && GetValue(y+1,x).handling_special == true)
				{
					struct puyodata temp;
					temp.color = GetValue(y,x).color;
					temp.handling = true;
					temp.handling_special = true;
					SetValue(y,x,temp);
					Move = false;
				}
				if (GetValue(y,x).handling_special == true && y == GetLine() -1)
				{
					struct puyodata temp;
					temp.color = GetValue(y,x).color;
					temp.handling = false;
					temp.handling_special = false;
					SetValue(y,x,temp);
					Move = false;
				}
				if (GetValue(y+1,x).handling_special == false)
				{
					if (GetValue(y,x).handling_special == true && GetValue(y+1,x).color != NONE)
					{
						struct puyodata temp;
						temp.color = GetValue(y,x).color;
						temp.handling = false;
						temp.handling_special = false;
						SetValue(y,x,temp);
						Move = false;
					}
				}
				if (GetValue(y,x).handling_special == false)
				{
					if (y == GetLine() - 1 || GetValue(y + 1,x).color != NONE)
					{
							struct puyodata temp;
                                        		temp.color = GetValue(y,x).color;
                                        		temp.handling = false;
                                        		SetValue(y,x,temp);
							Move = false;
					}
				}
			}
		}
	}
}

//下移動
void FieldControl::ControlDown()
{
	//一時的格納場所メモリ確保
	struct puyodata *field_array_temp;
	field_array_temp = new struct puyodata[GetLine()*GetColumn()];

            //1つ下の位置にfield_arrayからfield_array_tempへとコピー
	for (int y = GetLine()-1; -1 < y ; y--)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (GetValue(y,x).handling == true)
			{
				if (y < GetLine()-1)
				{

					field_array_temp[(y + 1)*GetColumn() + x] = GetValue(y,x);
				}
				else
				{
					field_array_temp[y*GetColumn() + x] = GetValue(y,x);
				}

				//コピー後にfield_arrayのデータは消す
                struct puyodata temp;
                temp.color = NONE;
                temp.handling = false;
                SetValue(y,x,temp);
			}
		}
	}

	//field_array_tempからfield_arrayへコピー
	for (int y = 0; y < GetLine(); y++)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (field_array_temp[y*GetColumn() + x].handling == true)
			{
				SetValue(y,x,field_array_temp[y*GetColumn() + x]);
			}
		}
	}

	//一時的格納場所メモリ解放
	delete[] field_array_temp;
}

//左移動
void FieldControl::ControlLeft()
{
	//一時的格納場所メモリ確保
	struct puyodata *field_array_temp;
	field_array_temp = new struct puyodata[GetLine()*GetColumn()];

	//1つ左の位置にfield_arrayからfield_array_tempへとコピー
	for (int y = 0; y < GetLine(); y++)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (GetValue(y,x).handling == true)
			{
				if (0 < x)
				{
					field_array_temp[y*GetColumn() + (x - 1)] = GetValue(y,x);
				}
				else
				{
					field_array_temp[y*GetColumn() + x] = GetValue(y,x);
				}

				//コピー後にfield_arrayのデータは消す
                struct puyodata temp;
                temp.color= NONE;
                temp.handling = false;
                SetValue(y,x,temp);
			}
		}
	}

	//field_array_tempからfield_arrayへコピー
	for (int y = 0; y < GetLine(); y++)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (field_array_temp[y*GetColumn() + x].handling == true)
			{
				SetValue(y,x,field_array_temp[y*GetColumn() + x]);
			}
		}
	}

	//一時的格納場所メモリ解放
	delete[] field_array_temp;
}

//右移動
void FieldControl::ControlRight()
{
	//一時的格納場所メモリ確保
	struct puyodata *field_array_temp;
	field_array_temp = new struct puyodata[GetLine()*GetColumn()];

	//1つ右の位置にfield_arrayからfield_array_tempへとコピー
	for (int y = 0; y < GetLine(); y++)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (GetValue(y,x).handling == true)
			{
				if (x <= GetColumn())
				{
					field_array_temp[y*GetColumn() + (x + 1)] = GetValue(y,x);
				}
				else
				{
					field_array_temp[y*GetColumn() + x] = GetValue(y,x);
				}

				//コピー後にfield_arrayのデータは消す
                struct puyodata temp;
                temp.color= NONE;
                temp.handling = false;
                SetValue(y,x,temp);

			}
		}
	}

	//field_array_tempからfield_arrayへコピー
	for (int y = 0; y < GetLine(); y++)
	{
		for (int x = 0; x < GetColumn(); x++)
		{
			if (field_array_temp[y*GetColumn() + x].handling == true)
			{
				SetValue(y,x,field_array_temp[y*GetColumn() + x]);
			}
		}
	}

	//一時的格納場所メモリ解放
	delete[] field_array_temp;
}


//表示
void Display(FieldControl& instans)
{
	//フィールド表示
	for (int y = 0; y < instans.GetLine(); y++)
	{
		for (int x = 0; x < instans.GetColumn(); x++)
		{
			switch (instans.GetValue(y, x).color)
			{
			case NONE:
				mvaddch(y, x, ' ');
				break;
			case RED:
                init_pair(1, COLOR_RED, COLOR_BLACK);
                attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
                init_pair(2, COLOR_BLUE, COLOR_BLACK);
                attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
                init_pair(3, COLOR_GREEN, COLOR_BLACK);
                attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
                init_pair(4, COLOR_YELLOW, COLOR_BLACK);
                attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}

		}
	}


	//情報表示
	int count = 0;
	for (int y = 0; y < instans.GetLine(); y++)
	{
		for (int x = 0; x < instans.GetColumn(); x++)
		{
			if (instans.GetValue(y, x).color != NONE)
			{
				count++;
			}
		}

	}


	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", instans.GetLine(), instans.GetColumn(),count);
	mvaddstr(2, COLS - 35, msg);

}


//ここから実行される
int main(int argc, char **argv){
    FieldControl fieldcontrol;
    
    
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();
    

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);


	//初期化処
    fieldcontrol.ChangeSize(LINES/2,COLS/2);
    fieldcontrol.GeneratePuyo();
    

	int delay = 0;
	int waitCount = 3000;

	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
           if (fieldcontrol.Move == true) {
               fieldcontrol.ControlLeft();
           }
                break;
		case KEY_RIGHT:
            if (fieldcontrol.Move == true) {
                fieldcontrol.ControlRight();
            }
			break;
		case 'Z':
			//ぷよ回転処理など
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			//ぷよ下に移動
			fieldcontrol.ControlDown();
			fieldcontrol.LandingPuyo();
			//ぷよ着地判定
			if (fieldcontrol.FallPuyo() == true){
				if (fieldcontrol.VanishPuyo() != 0)
                        		fieldcontrol.Move = false;
			}
			if (fieldcontrol.FallPuyo() == true){
				//着地したら新しいぷよ生成
				fieldcontrol.GeneratePuyo();
			}
		}
		
		/*
		if (fieldcontrol.FallPuyo() == true){
			if (fieldcontrol.VanishPuyo() != 0)
				fieldcontrol.Move = false;
		}
		*/
		delay++;


		//表示
		Display(fieldcontrol);
	}


	//画面をリセット
	endwin();

	return 0;
}
