#include <iostream>
#include <stdlib.h>
#include <stdio.h>

class vector
{
public:
	// 要素数を指定して初期化するコンストラクタ
	vector( int n ) : num( n <= 0 ? 0 : n ), x( n <= 0 ? NULL : new int[ num ] ){
		for( int i = 0; i < num; i++ ) x[ i ] = 0;
	}

	// コピーコンストラクタ（関数の戻り値や引数で生成される一時変数等で利用される）
	vector( const vector &v ): num( v.num ), x( new int[ num ] ){
		for( int i = 0; i < num; i++ ) x[ i ] = v.x[ i ];
	}

	// デフォルトコンストラクタ
	vector( ) : num( 0 ), x( NULL ){ }

	// デストラクタ（インスタンスが消滅する際に実行される）
	~vector( ){ delete [] x; }

public:
	// ベクトルの要素数を返す関数
	int size( ) const { return( num ); }

	// ベクトルの要素にアクセスするための[]演算子のオーバーロード
	int& operator []( int index ){ return( x[index] ); }
	// ベクトルの要素にアクセスするための[]演算子のオーバーロード（const版）
	const int& operator []( int index ) const { return( x[index] ); }

	// ベクトル同士の足し算を実現する+演算子のオーバーロード
	const vector operator +( const vector &b ) const {
		const vector &a = *this;
		if( a.size( ) != b.size( ) )
		{
			printf( "要素数が異なります" );
			exit( 1 );
		}
		vector t( a.size( ) );
		for( int i = 0; i < a.size( ); i++ ) t[ i ] = a[ i ] + b[ i ];
		return( t );
	}

	// ベクトル同士の引き算を実現する-演算子のオーバーロード
	const vector operator -( const vector &b ) const {
		
		const vector &a = *this;
		if( a.size( ) != b.size( ) )
		{
			printf( "要素数が異なります" );
			exit( 1 );
		}

		vector t( a.size( ) );
		for( int i = 0; i < a.size( ); i++ ) t[ i ] = a[ i ] - b[ i ];
		return( t );
	}

	// ベクトルの符号反転したものを返す-演算子のオーバーロード
	const vector operator -( ) const {
		const vector &a = *this;

		vector t( a.size( ) );
		for( int i = 0; i < a.size( ); i++ ) t[ i ] = -a[ i ];
		return( t );
	}

	// ベクトル同士の全要素が同じ場合にtrueを返し，それ以外はfalseを返すベクトル同士の比較関数
	bool operator ==( const vector &b ) const {
		const vector &a = *this;
		if( a.size( ) != b.size( ) )
		{
			printf( "要素数が異なります" );
			exit( 1 );
		}

		bool flag = true;
		for( int i = 0; i < a.size( ); i++ ) flag = flag && a[ i ] == b[ i ];
		return( flag );
	}

	// 他のベクトルオブジェクトを代入する演算子のオーバーロード
	void operator =( const vector &b ){
		delete [] x;
		x = new int[ b.size( ) ];
		num = b.size( );
		for( int i = 0; i < num; i++ ) x[ i ] = b[ i ];
	}

	// ベクトルの中身を表示する関数
	virtual void print( ) const {
		const vector &a = *this;
		if( a.size( ) == 0 ) printf( "Empty\n" );
		for( int i = 0; i < a.size( ); i++ ) std::cout << a[ i ] << std::endl;
	}

protected:
	int num;	// 要素数を保持するメンバ変数
	int* x;		// 要素のメモリ領域を指すポインタ
};

class matrix : public vector
{
public:
	// r行c列の行列として初期化するコンストラクタ
	matrix( int r, int c ) : vector( r * c ), num1( r ), num2( c ){}

	// デフォルトコンストラクタ
	matrix( ) : vector( ){ }

public:
	// 行数を返す関数
	int rows( ) const { return( num1 ); }

	// 列数を返す関数
	int cols( ) const { return( num2 ); }

	// r行c列の要素にアクセスするための()演算子のオーバーロード
	int& operator ()( int r, int c ){ return( this->operator[]( r + c * num2 ) ); }
	// r行c列の要素にアクセスするための()演算子のオーバーロード（const版）
	const int& operator ()( int r, int c ) const { return( this->operator[]( r + c * num2 ) ); }

protected:
	int num1;	// 行数を保持する変数
	int num2;	// 行数を保持する変数
};

void print( const vector &v ){ v.print( ); }

int main()
{
	matrix m( 3, 3 );
	m( 0, 0 ) = m( 1, 1 ) = m( 2, 2 ) = 1;
	print( m );
	return 0;
}
