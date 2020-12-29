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
//#define VERSION "VER 1.4 　txtファイル出力機能追加。 2019/11/28"
  #define VERSION "VER 2.0 　重複チェックを4ByteSumからCRC32へ変更。 2019/12/06"

using namespace std;

#ifdef _DEBUG
	#define F5DEBUG
#endif

static const unsigned int crc32tab[256] = { 
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};


unsigned int crc32(char *p, int len)
{
	unsigned int crcinit = 0;
	unsigned int crc = 0;

	crc = crcinit ^ 0xFFFFFFFF;
	for (; len--; p++) {
		crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32tab[(crc ^ (*p)) & 0xFF];
	}
	return crc ^ 0xFFFFFFFF;
}


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
	unsigned long fileSize;	// doubleは出力時に%fを使う。整数部だけ出したかったら%0.0f
							// long longはめっちゃでかくできるが、パス名の出力ができなくなってしまった（nullになる。未調査）そもそも4GiByteまでしかよめないはずなのでunsigned longにする
	bool base;
	bool willDel;
} _OneFile;


int main(int argc, char* argv[])
{
	// --memo--
	// cinとgetchar()を使うと、cinの最後に行われるenterキー入力がバッファに残るようで、enterがgetcharに渡されてしまう。（Windowsの改行コードがCRLFだから？）
	// ｃ++のストリームcinを使うならcの標準入出力と併用しないべき。printfもcoutにすべきかも。
	// getchar()で止めるのではなく、system("pause");をつかう。
	//
	//printf("double:%d long:%d unsigned long:%d DWORD:%d WORD:%d unsigned int:%d int:%d short:%d unsigned short:%d\n",
	//	sizeof(double), sizeof(long), sizeof(unsigned long), sizeof(DWORD), sizeof(WORD), sizeof(unsigned int), sizeof(int), sizeof(short), sizeof(unsigned short));
	// DWORDやsize_t型は、cpuによって変化する型のサイズに対応するために定義されている。
	// 処理系が違うと、intやlongのサイズが異なり意図通りに動かなくなる可能性があるが、それらを使っていれば問題が発生しにくいはず。
	// CRC計算を行う際に4byteや8byteの型を必ずそのサイズで指定するためにはどうすべきなのだろう？8byteならdoubleがかならず8byteだが、4byteが保証されている型は存在する？

	printf("%s\n\n",VERSION);
	cout << "CRC32とファイルサイズからファイルの重複チェックをします。\n削除しますか？\ny/n( y or Y 以外はチェックのみ実行)"<<endl;
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
	cout << " listup Finish. FileNum[" << folderAllFile_str.size() << "]" << endl;
	vector<string>::iterator vec_itr_folderAllFile_str = folderAllFile_str.begin();     // String folderAllFile_strのイテレータ

	byte* readBuff = NULL;	// ここに丸ごと読み込んで処理する。
	_OneFile fileData;
	memset(&fileData, 0x00, sizeof(fileData) );
	
	list<_OneFile> filelist;				// リストとして持つ
	list<_OneFile>::iterator itr_base;		// 重複チェック元イテレータ
	list<_OneFile>::iterator itr_check;		// 重複チェック対象イテレータ

	int sumCnt=1;
	cout << "Make CRC32....";
	//DWORD fsize  = 0;
	//DWORD fsizeb = 0;
	fpos_t fsize  = {0};
	fpos_t fsizeb = {0};
	
	unsigned int crc =0;
	char* cp = NULL;

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
		fsize.__offset = 0;
		fsize.__value = 0;
		fsizeb.__offset = 0;
		fsizeb.__value = 0;

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
		fsize.__value  = 0;
		fsize.__offset  = 0;
		fsizeb.__offset =  fseek(readFp,0,SEEK_END);
		fgetpos(readFp,(fpos_t*)&fsize);
		fseek(readFp,fsizeb.__offset,SEEK_SET);
		fsize.__offset++;	// ファイル終端

		// メモリに読む
		readBuff = NULL;	//Logをここに丸ごと読み込んで処理する。
		// 小さいファイルの処理を行う際、debug環境ならsumが一致するが、release環境だとsumが異なってしまう。
		// ブロックかセクタサイズに影響を受け、初期化されていない領域のデータも計算してしまうと思われる。
		if(fsize.__value<4*1024)
		{
			readBuff = new BYTE[4*1024];//(byte *)malloc( 4*1024 );
			memset( readBuff, 0, 4*1024 );
		}
		else
		{
			readBuff = new BYTE[fsize.__value];//(byte *)malloc( fsize );
			memset( readBuff, 0, fsize.__value );
		}
		fread( readBuff, 1, fsize.__value, readFp );
		//printf("                                                                                                              \r");
		//printf("make Sum[%s]\r",vec_itr_folderAllFile_str->c_str());
		if(0 == sumCnt%100)
		{
			cout << "|";
		}
		fclose(readFp);

//		// Sum計算
//		DWORD* dwp=(DWORD*)readBuff;
//		DWORD checkSize = fsize;
//		// cszに対象バイト数が入る。データが10byteなら、4Byte単位のsumをとるためには12byteとして扱う必要があるため、12が入る。
//		// 13なら16。16は16。17は20
//		checkSize =(DWORD)( fsize + sizeof(DWORD) - 1 );
//		checkSize/=sizeof(DWORD);
//		checkSize*=sizeof(DWORD);
//
//		DWORD ret=0;
//		//↓cszにチェックサムとして計算するサイズが入っている。データが10Byteなら12が入っているなので、DWORDで割ると3回実行される。
//		//↓ポインタが3回加算されると4Byte * 3になる
//		//最大3Byte後ろにアクセスするので、dwpが指し示すreadbufの領域は、ファイルサイズよりも最低3Byte多く確保されていなければならない
//		for(DWORD i=0; i < ( checkSize/sizeof(DWORD) ); i++ )
//		{
//			ret+=*dwp;
//			dwp++;
//		}

		// crc32
		//cp = (char*)readBuff;
		//crc = crc32(cp, fsize-1);

		// 情報入れてList生成
		fileData.pass = *vec_itr_folderAllFile_str;
//		fileData.sum = ret;
		fileData.sum = crc32((char*)readBuff, fsize.__value-1);
		//printf("sum:%x\tName:%s \n", fileData.sum, fileData.pass.c_str() );
		fileData.fileSize = fsize.__value-1;
		
		filelist.push_back(fileData);
		vec_itr_folderAllFile_str++;

		delete[] readBuff;//free(readBuff);
		readBuff = NULL;
		sumCnt++;
	}

	// リスト作成終了
	printf(" Make CRC32 Finish. FileNum[%d]\n",filelist.size() );

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
		printf(        "Check cnt[%03d]  Search CRC32[%08x] size[%dbyte] file[%hs]\n",checkCnt++, itr_base->sum, itr_base->fileSize, itr_base->pass.c_str() );
		//printf("num[%02d] Search sum:%x [%s]\n",filelist.size(), itr_check->sum, itr_check->pass.c_str() );
		while(/*1*/filelist.end() != itr_check)
		{
			//printf("%dkaime---------------------------\n",kai++);
			//printf("Check sum:%x pass:%s\n",itr_check->sum, itr_check->pass.c_str() );
			if( (itr_base->sum == itr_check->sum) && (itr_base->fileSize == itr_check->fileSize) )
			{
				if( ( 0 == strcmp("Y", YorN.c_str() ) ) || 
					( 0 == strcmp("y", YorN.c_str() ) ) )
				{
					// 背景色
					wAttributes = FOREGROUND_RED;
					SetConsoleTextAttribute(hStdout, wAttributes);

					removeRet = remove(itr_check->pass.c_str() );
					printf("Find!!      delete!    CRC32[%08x] size[%dbyte] file[%s] ret:%x\n",itr_check->sum, itr_check->fileSize, itr_check->pass.c_str(), removeRet );
					itr_check = filelist.erase(itr_check);
					delcnt++;
				}
				else
				{
					// 背景色
					wAttributes = BACKGROUND_GREEN;
					SetConsoleTextAttribute(hStdout, wAttributes);

					printf("Find!! will be delete  CRC32[%08x] size[%dbyte] file[%s] ret:%x\n",itr_check->sum, itr_check->fileSize, itr_check->pass.c_str(), removeRet );
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
						fprintf( outfp, "[Delete]\t[%08x]\t[%dbyte]\t%s\n", itr_check->sum, itr_check->fileSize,  itr_check->pass.c_str() );
						//cout << itr_check->pass
					}
					else
					{
						fprintf( outfp, "[Base  ]\t[%08x]\t[%dbyte]\t%s\n", itr_check->sum, itr_check->fileSize, itr_check->pass.c_str() );
					}
				}
				else if(true == itr_check->willDel)
				{
					fprintf( outfp, "[Delete]\t[%08x]\t[%dbyte]\t%s\n", itr_check->sum, itr_check->fileSize, itr_check->pass.c_str() );
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
	return 0;
}
