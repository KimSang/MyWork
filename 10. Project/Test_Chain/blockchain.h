#ifndef TESTBLOCK_BLOCKCHAIN_H
#define TESTBLOCK_BLOCKCHAIN_H

#include <cstdint>
#include <vector>
#include "block.h"

using namespace std;

class Blockchain
{
	public:
		Blockchain();

		void AddBlock(Block bNew);
		
	private:

		uint32_t _nDifficulty;

		vector<Block> _vChain;

		Block _GetLastBlock() const;
		
};

#endif
