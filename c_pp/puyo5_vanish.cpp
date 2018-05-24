
public:

	//ぷよ消滅処理を全座標で行う
	//消滅したぷよの数を返す
	int VanishPuyo()
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
	int VanishPuyo(unsigned int y, unsigned int x)
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

						vanishednumber++;
					}
				}
			}
		}

		//メモリ解放
		delete[] field_array_check;

		return vanishednumber;
	}
