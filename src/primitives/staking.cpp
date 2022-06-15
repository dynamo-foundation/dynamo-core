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

    int bestHeight;
    
    {
    LOCK(cs_main);
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

            //need staking database here - has to be fast
            //need ticket database here

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
