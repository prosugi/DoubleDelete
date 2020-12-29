//#define __STDC_WANT_LIB_EXT1__ 1

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <cstdio>


//#define VERSION "VER 1.2 　下層フォルダも再起チェックし、同一ファイルを削除する。"
//#define VERSION "VER 1.3 　下層フォルダも再起チェックし、Openエラーで止まらないように変更。 2019/11/26"
#define VERSION "VER 1.4 　txtファイル出力機能追加。 2019/11/28"

using namespace std;

#ifdef _DEBUG
	#define F5DEBUG
#endif

class Dir
{
public:
	// コンストラクタ
	Dir(void){}
	// デストラクタ
	virtual ~Dir(void){}

	// ファイル一覧取得
	// folder : フォルダの絶対パスを入力とする 
	// 例 : "D:\\Users\\Pictures\\"
	static void read(string folder, vector<string>* fileList)
	{
		// 宣言
		//vector<string> fileList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// ファイル名検索のためにワイルドカード追加
		// 例 : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if(*itr != '\\') ss << '\\';
		ss << "*.*";

		// ファイルのフルパス格納用
		string fullpass;

		// ファイル探索
		// FindFirstFile(ファイル名, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		int strcnt =0;

		// 検索失敗
		if(hFind == INVALID_HANDLE_VALUE){
			cout << "ファイル一覧を取得できませんでした" << endl;
			cout << "folder:" << folder;
			//getchar();
			system("pause");
			exit(1); // エラー終了
		}

		int dispCnt=1;
		// ファイル名をリストに格納するためのループ
		do{
			//char *file = (char*)fd.cFileName;
			//string str = file;
			//fileList.push_back(str);
			fullpass = folder;
			fullpass +='\\';
			fullpass += (char*)fd.cFileName;
			//printf("fd.cFileName:%s\n",fd.cFileName);
			
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			  && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				//ファイル名をリストに格納
				//printf("                                                                                                              \r");
				//printf("ListPush  [%s]\r",(char*)fd.cFileName);
				fileList->push_back(fullpass);
			}
			else if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				      0 != strcmp(fd.cFileName,"..") &&
					  0 != strcmp(fd.cFileName,".") )
			{
				// フォルダは再帰検索
				//printf("                                                                                                              \r");
				//printf("OpenFolder[%s]\r",(char*)fd.cFileName);
				read(fullpass, fileList);
			}
			if(0 == dispCnt%100)
			{
				printf("|");
			}
			dispCnt++;
		}while(FindNextFile(hFind, &fd)); //次のファイルを探索
		
		// hFindのクローズ
		FindClose(hFind); 

		//return fileList;
		return;
	}
};

// 上の使い方
//int main(int argc, char *argv[])
//{
//	// ファイル入力
//	std::vector<std::string> folderAllFile_str = Dir::read("D:\\Users\\Pictures\\");
//	for(int i = 0; i < folderAllFile_str.size(); i++){
//		std::cout << folderAllFile_str[i] << std::endl;
//	}
//	return 0;
//}

typedef struct {
    string pass;
    unsigned int sum;
	bool base;
	bool willDel;
} _OneFile;


int main(int argc, char* argv[])
{
	// memo
	// cinとgetchar()を使うと、cinの最後に行われるenterキー入力がバッファに残るようで、enterがgetcharに渡されてしまう。（Windowsの改行コードがCRLFだから？）
	// ｃ++のストリームcinを使うならcの標準入出力と併用しないべき。printfもcoutにすべきかも。
	// getchar()で止めるのではなく、system("pause");をつかう。
	printf("%s\n\n",VERSION);
	cout << "4ByteのSumをとり、ファイルの重複チェックをします。\n削除しますか？\ny/n( y or Y 以外はチェックのみ実行)"<<endl;
	string YorN;
	cin >> YorN;

	if( ( 0 == strcmp("Y", YorN.c_str() ) ) || 
	( 0 == strcmp("y", YorN.c_str() ) ) )
	{
		cout << endl << "-----Delete Start-----" << endl << endl;
	}
	else
	{
		cout << endl << "+++++Check Only++++++" << endl << endl;
	}
	
	#ifdef F5DEBUG
	// F5で実行するとフォルダパスに含まれるスペース毎にargvに格納される。argcもxが入る。
	// そのため、ここで成形してdebugする。
    // コンパイラのインストールフォルダによるだろうが。
	if(2!=argc)
	{
		printf("\nデバッグON\n");
		char* debug_path;
		debug_path = new char[1000];
		memset(debug_path, 0,sizeof(debug_path));
		strcat(debug_path, argv[1]);
		strcat(debug_path, " ");
		strcat(debug_path, argv[2]);
		strcat(debug_path, " ");
		strcat(debug_path, argv[3]);
		strcpy(argv[1], debug_path);
		delete[] debug_path;
	}
	#endif

	// 文字色変更用
	HANDLE hStdout;
	WORD wAttributes;
	CONSOLE_SCREEN_BUFFER_INFO csbi;//構造体です
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	//hStdoutのコンソールスクリーンバッファ情報をcsbiに取得
	GetConsoleScreenBufferInfo(hStdout, &csbi);

	if(1 >= argc)
	{
		cout << "フォルダをドロップすると動作します。" << endl;
		system("pause");
		return 0;
	}
	else
	{
		cout <<"["<< argv[1] <<"]" << endl;
	}
	// ファイル読込む
	vector<string> folderAllFile_str;
	cout <<"listup file...";
	Dir::read(argv[1], &folderAllFile_str);
	cout << " listup End\n";
	vector<string>::iterator vec_itr_folderAllFile_str = folderAllFile_str.begin();     // String folderAllFile_strのイテレータ

	byte* logBuff = NULL;	// ここに丸ごと読み込んで処理する。
	_OneFile fileData;
	memset(&fileData, 0x00, sizeof(fileData) );
	
	list<_OneFile> filelist;				// リストとして持つ
	list<_OneFile>::iterator itr_base;		// 重複チェック元イテレータ
	list<_OneFile>::iterator itr_check;	// 重複チェック対象イテレータ

	int sumCnt=1;
	cout << "Make Sum......";
	while(folderAllFile_str.end() != vec_itr_folderAllFile_str)
	{
		// ファイル入力 ★下の条件でwhile文回す？
		//for(int i = 0; i < folderAllFile_str.size(); i++){
		//	cout << folderAllFile_str[i] << endl;
		//}

		// ファイルオープンして読み込む
		FILE *readFp = NULL; 
		//fpos_t fsize  = 0;
		//fpos_t fsizeb = 0;
		DWORD fsize  = 0;
		DWORD fsizeb = 0;

		//printf("                                                                                                              \r");
		//printf("open[%s]\r",vec_itr_folderAllFile_str->c_str());
		//int fopenRet = fopen_s(&readFp, vec_itr_folderAllFile_str->c_str(), "rb");
		readFp = fopen( vec_itr_folderAllFile_str->c_str(), "rb");
		//printf( "「%s」から抽出\n", argv[FinFileNum] );
		if(readFp == NULL)
		{
			//例外
			//printf( "%s\nのファイルオープン失敗 [%d]\n", vec_itr_folderAllFile_str->c_str(), fopenRet );
			printf( "%s\nのファイルオープン失敗\n", vec_itr_folderAllFile_str->c_str() );

			//getchar();// 結果見せるため止める
			//system("pause");
			//return -1;
			
			// 処理止めずに進める
			vec_itr_folderAllFile_str++;
			continue;
		}
		else
		{
			//printf( "%s\nのファイルオープン成功\n", vec_itr_folderAllFile_str->c_str() );
			//getchar();// 結果見せるため止める
		}

		//ドロップされたファイル名取得
		//_splitpath_s(argv[FinFileNum], drive, sizeof(drive), dir, sizeof(dir), FileName, sizeof(FileName), ext, sizeof(ext));

		// サイズ取得
		fsize  = 0;
		fsizeb =  fseek(readFp,0,SEEK_END);
		fgetpos(readFp,(fpos_t*)&fsize);
		fseek(readFp,fsizeb,SEEK_SET);
		fsize++;	// ファイル終端

		// メモリに読む
		logBuff = NULL;	//Logをここに丸ごと読み込んで処理する。
		// 小さいファイルの処理を行う際、debug環境ならsumが一致するが、release環境だとsumが異なってしまう。
		// ブロックかセクタサイズに影響を受け、初期化されていない領域のデータも計算してしまうと思われる。
		if(fsize<4*1024)
		{
			logBuff = new BYTE[4*1024];//(byte *)malloc( 4*1024 );
			memset( logBuff, 0, 4*1024 );
		}
		else
		{
			logBuff = new BYTE[fsize];//(byte *)malloc( fsize );
			memset( logBuff, 0, fsize );
		}
		fread( logBuff, 1, fsize, readFp );
		//printf("                                                                                                              \r");
		//printf("make Sum[%s]\r",vec_itr_folderAllFile_str->c_str());
		if(0 == sumCnt%100)
		{
			cout << "|";
		}
		fclose(readFp);

		// Sum計算
		DWORD* dwp=(DWORD*)logBuff;
		DWORD checkSize = fsize;
		// cszに対象バイト数が入る。データが10byteなら、4Byte単位のsumをとるためには12byteとして扱う必要があるため、12が入る。
		// 13なら16。16は16。17は20
		checkSize =(DWORD)( fsize + sizeof(DWORD) - 1 );
		checkSize/=sizeof(DWORD);
		checkSize*=sizeof(DWORD);

		DWORD ret=0;
		//↓cszにチェックサムとして計算するサイズが入っている。データが10Byteなら12が入っているなので、DWORDで割ると3回実行される。
		//↓ポインタが3回加算されると4Byte * 3になる
		//最大3Byte後ろにアクセスするので、dwpが指し示すreadbufの領域は、ファイルサイズよりも最低3Byte多く確保されていなければならない
		for(DWORD i=0; i < ( checkSize/sizeof(DWORD) ); i++ )
		{
			ret+=*dwp;
			dwp++;
		}

		// 情報入れてList生成
		fileData.pass = *vec_itr_folderAllFile_str;
		fileData.sum = ret;
		//printf("sum:%x\tName:%s \n", fileData.sum, fileData.pass.c_str() );
		
		filelist.push_back(fileData);
		vec_itr_folderAllFile_str++;

		delete[] logBuff;//free(logBuff);
		logBuff = NULL;
		sumCnt++;
	}

	// リスト作成終了
	printf(" Make Sum Finish. FileNum[%d]\n",filelist.size() );

DELDEL:

	// 先頭ファイルから重複チェック
	itr_base = filelist.begin();

	int delcnt = 0;
	int checkCnt = 1;
	int kai=1;
	int removeRet=0;
	while(filelist.end() != itr_base)
	{
		itr_check = itr_base;
		itr_check++;
		if(filelist.end() ==itr_check)
		{
			// チェック対象の次のファイルから確認するが、番兵に当たったら終了
			//printf("Check iter is over. list.end() is not last date. it is Banpei.\n");
			break;
		}
		printf(        "Check cnt[%03d]  Search sum[%08x] file[%s]\n",checkCnt++, itr_base->sum, itr_base->pass.c_str() );
		//printf("num[%02d] Search sum:%x [%s]\n",filelist.size(), itr_check->sum, itr_check->pass.c_str() );
		while(/*1*/filelist.end() != itr_check)
		{
			//printf("%dkaime---------------------------\n",kai++);
			//printf("Check sum:%x pass:%s\n",itr_check->sum, itr_check->pass.c_str() );
			if(itr_base->sum == itr_check->sum)
			{
				if( ( 0 == strcmp("Y", YorN.c_str() ) ) || 
					( 0 == strcmp("y", YorN.c_str() ) ) )
				{
					// 背景色
					wAttributes = FOREGROUND_RED;
					SetConsoleTextAttribute(hStdout, wAttributes);

					removeRet = remove(itr_check->pass.c_str() );
					printf("Find!!      delete!    sum[%08x] file[%s] ret:%x\n",itr_check->sum, itr_check->pass.c_str(), removeRet );
					itr_check = filelist.erase(itr_check);
					delcnt++;
				}
				else
				{
					// 背景色
					wAttributes = BACKGROUND_GREEN;
					SetConsoleTextAttribute(hStdout, wAttributes);
					printf("Find!! will be delete  sum[%08x] file[%s] ret:%x\n",itr_check->sum, itr_check->pass.c_str(), removeRet );
					itr_base->base = true;
					itr_check->willDel = true;
					itr_check++;
					delcnt++;
				}

				//元のテキスト状態に戻す
				SetConsoleTextAttribute(hStdout, csbi.wAttributes);
			}
			else
			{
				//printf("not del                sum[%08x] file[%s]\n",itr_check->sum, itr_check->pass.c_str() );
				itr_check++;
			}
			if(filelist.end() == itr_check)
			{
				break;
			}
		}
		if(filelist.end() == itr_base)
		{
			//ありえなくね？
			//printf("BREAK!!!!!!!!!!!!!!!!!!!!\n");
			printf("in while itr_base is end()\n");
			//getchar();
			break;
		}
		itr_base++;
	}
	printf("Check:%d\n  Del:%d\n", folderAllFile_str.size(), delcnt);

	if( ( 0 != strcmp("Y", YorN.c_str() ) ) &&
		( 0 != strcmp("y", YorN.c_str() ) ) )
	{
		cout << endl << "ファイルの重複チェック終了。\n重複ファイルを削除しますか？\ny/n/f\n( y or Y 以外は終了。fは削除予定ファイルをexeフォルダにtext出力して終了します。)"<<endl;
		string del;
		cin >> del;
		if( ( 0 == strcmp("Y", del.c_str() ) ) || 
			( 0 == strcmp("y", del.c_str() ) ) )
		{
			//YorN.erase(YorN.begin(),YorN.end());
			//YorN += "Y";
			YorN = "Y";
			goto DELDEL;
		}
		else if( ( 0 == strcmp("F", del.c_str() ) ) || 
				 ( 0 == strcmp("f", del.c_str() ) ) )
		{
			char drive[_MAX_DRIVE]; /* _MAX_DRIVE 3 */
			char dir[_MAX_DIR]; /* _MAX_DIR256 */
			char name[_MAX_FNAME]; /* _MAX_FNAME256 */
			char ext[_MAX_EXT]; /* _MAX_EXT256 */
			_splitpath(argv[0], drive, dir, name, ext);

			char outPathString[_MAX_PATH*4];
			//printf("00 onlyPathString %s clip = %d\n",onlyPathString,clip);
			memset(outPathString, 0, sizeof(outPathString));
			strcat(outPathString, drive);
			strcat(outPathString, dir);
			strcat(outPathString, "DbDl.txt");
			
			FILE *outfp;
			outfp = fopen( outPathString, "w" );
			//printf("fname = %s\n",fname);
			//outfp = fopen( fname, "w" );
			if( outfp == NULL )
			{
				printf( "出力用「%s」が開けない。開いてる?\n", outPathString );
				system("pause");//結果見せるため止める
				return -1;
			}

			itr_check = filelist.begin();
			while( filelist.end() != itr_check)
			{
				if(true == itr_check->base)
				{
					if(true == itr_check->willDel)
					{
						// 削除されていないことによる重複チェックファイルのため削除される側
						fprintf( outfp, "[Delete][%08x]\t%s\n", itr_check->sum,  itr_check->pass.c_str() );
						//cout << itr_check->pass
					}
					else
					{
						fprintf( outfp, "[Base  ][%08x]\t%s\n", itr_check->sum, itr_check->pass.c_str() );
					}
				}
				else if(true == itr_check->willDel)
				{
					fprintf( outfp, "[Delete][%08x]\t%s\n", itr_check->sum, itr_check->pass.c_str() );
				}
				itr_check++;
			}
			fclose(outfp);
			cout << outPathString <<"作成完了。" << endl;
			system("pause");
		}
	}
	else
	{
		system("pause");
	}
}
