#ifndef QUADRATIC_PROBING_H
#define QUADRATIC_PROBING_H

#include <vector>
#include <string>
#include <fstream>
using namespace std;

int nextPrime( int n );
int hash1( const string & key );
int hash1( int key );

// QuadraticProbing Hash table class
//
// CONSTRUCTION: an approximate initial size or default of 101
//
// ******************PUBLIC OPERATIONS*********************
// bool insert( x )       --> Insert x
// bool remove( x )       --> Remove x
// bool contains( x )     --> Return true if x is present
// void makeEmpty( )      --> Remove all items
// int hash( string str ) --> Global method to hash strings

template <typename HashedObj>
class HashTable
{
  public:
    explicit HashTable( int size = 101 ) : array( nextPrime( size ) )
      { makeEmpty( ); }

    bool contains( const HashedObj & x ) const
    {
        return isActive( findPos( x , false) );
    }

	void stats(vector<int> & x)
	{
		ofstream fout;
		fout.open("stats.txt");

		int maxChain = 0, sum = 0;

		for (int i = 0; i < x.size(); i++)
		{
			if (x[i] > maxChain)
				maxChain = x[i];

			sum += x[i];
		}

		fout << "Number of words: " << currentSize << endl;
		fout << "Table size: " << array.size() << endl;
		fout << "Load factor: " << double(currentSize) / array.size() << endl;
		fout << "Collisions: " << collisions << endl;
		fout << "Average chain length: " << double(sum) / x.size() << endl;
		fout << "Longest chain length: " << maxChain << endl;
	}

    void makeEmpty( )
    {
        currentSize = 0;
        for( int i = 0; i < array.size( ); i++ )
            array[ i ].info = EMPTY;
    }

    bool insert( const HashedObj & x )
    {
            // Insert x as active
        int currentPos = findPos( x, true );
        if( isActive( currentPos ) )
            return false;

        array[ currentPos ] = HashEntry( x, ACTIVE );

            // Rehash; see Section 5.5
        if( ++currentSize > array.size( ) / 2 )
            rehash( );

        return true;
    }

    bool remove( const HashedObj & x )
    {
        int currentPos = findPos( x , false);
        if( !isActive( currentPos ) )
            return false;

        array[ currentPos ].info = DELETED;
        return true;
    }

    enum EntryType { ACTIVE, EMPTY, DELETED };

  private:
    struct HashEntry
    {
        HashedObj element;
        EntryType info;

        HashEntry( const HashedObj & e = HashedObj( ), EntryType i = EMPTY ) : element( e ), info( i ) { }
    };
    
    vector<HashEntry> array;
    int currentSize;
	int nWords = 0;
	mutable int collisions = 0;	  // mutable allows variable to be changed in const function

    bool isActive( int currentPos ) const
      { return array[ currentPos ].info == ACTIVE; }

    int findPos( const HashedObj & x , bool collision) const
    {
        int offset = 1;
        int currentPos = myhash( x );
		int coll = 0;

		// If there's already an element in the bucket, increment collisions
		if (array[currentPos].info != EMPTY && collision)
			collisions++;

          // Assuming table is half-empty, and table length is prime,
          // this loop terminates
        while( array[ currentPos ].info != EMPTY && array[ currentPos ].element != x )
        {   
			currentPos += offset;  // Compute ith probe
            offset += 2;
            if( currentPos >= array.size( ) )
                currentPos -= array.size( );
        }
        return currentPos;
    }

    void rehash( )
    {
        vector<HashEntry> oldArray = array;

        // Create new double-sized, empty table
        array.resize( nextPrime( 2 * oldArray.size( ) ) );

        for( int j = 0; j < array.size( ); j++ )
            array[ j ].info = EMPTY;

        // Copy table over
        currentSize = 0;
		collisions = 0;
        for( int i = 0; i < oldArray.size( ); i++ )
            if( oldArray[ i ].info == ACTIVE )
                insert( oldArray[ i ].element );

    }

    int myhash( const HashedObj & x ) const
    {
        int hashVal = hash1( x );

        hashVal %= array.size( );
        if( hashVal < 0 )
            hashVal += array.size( );

        return hashVal;
    }
};

#endif