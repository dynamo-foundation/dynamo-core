#include <primitives/staking.h>



CStaking::CStaking() {
    lastEpochPaid = -1;
}

void CStaking::processStakeCommand(std::string address, std::string txid, uint32_t length, char type, uint64_t amount) {



}

void CStaking::processUnstakeCommand(std::string txid) {
}

void CStaking::processReceiptClaim() {
}


//called each time a block is added to the chain
void CStaking::processRewardPayout() {

    //as each block is added, check the best tip chain height
    //if the height - cooldown mod the epoch = 0 then process payouts
    //payouts are processed in consecutive blocks by mod with the payout modulus

    //block vaidation forces miners to include POS rewards, so all pending rewards need to be built from genesis

    int bestHeight;
    
    {
    LOCK(cs_main);
    /*
    if (::ChainstateActive().IsInitialBlockDownload())
        return;
        */
    //::ChainActive().Tip()->GetBlockHash().GetHex();
    bestHeight = ::ChainActive().Tip()->nHeight;
    }

    int lastEpoch = (bestHeight % STAKING_EPOCH) - 1;
    int startOfLastEpoch = lastEpoch * STAKING_EPOCH;
    int endOfLastEpoch = lastEpoch * STAKING_EPOCH + STAKING_EPOCH - 1;

    if (bestHeight >= endOfLastEpoch + STAKING_CALC_COOLDOWN) {
        //do we need to calculate payouts?
        if (lastEpoch != lastEpochPaid) {
            //calculate all rewards that are due for payment

            //each ticket will be validated at time of submission - no need to revalidate saking power
            //scan last epoch of blocks for ticket claims

            CBlock block;
            CBlockIndex* pblockindex;

            //read all blocks from last epoch
            for (int blockNum = startOfLastEpoch; blockNum <= endOfLastEpoch; blockNum++)
                {
                LOCK(cs_main);
                pblockindex = ::ChainActive()[blockNum];
                ReadBlockFromDisk(block, pblockindex, Params().GetConsensus());
                //for each block, sum all tickets

                }
   

            /*
            *
            * 
    return pblockindex->GetBlockHash().GetHex();

    {
        LOCK(cs_main);
        pblockindex = g_chainman.m_blockman.LookupBlockIndex(hash);
        tip = ::ChainActive().Tip();

        if (!pblockindex) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");
        }

    }
            */
            

            payoutIntervalCounter = 0;
            lastEpochPaid = lastEpoch;
        }
        else {
            if (payoutIntervalCounter < STAKING_PAYOUT_INTERVAL) {
                //scan remaining rewards payouts - were txidLSB mod the interval counter = 0, make the payment and remove from the list

            }
        }
    }

}


void CStaking::issueReceiptClaim() {
}
