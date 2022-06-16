#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <map>

#include <chain.h>
#include <chainparams.h>
#include <consensus/consensus.h>
#include <consensus/params.h>
#include <consensus/validation.h>
#include <validation.h>
#include <validationinterface.h>

#define STAKING_EPOCH           100         //number of blocks to calculate rewards over

#define STAKING_CALC_COOLDOWN   20          //number of blocks after epoch end to wait for rewards payout (to avoid deep forks)

#define STAKING_RECEIPT_MOD     64          //modulus to use for ticket claim
                                            //if this is too large, some blocks will have 0 tickets, which might be ok
                                            //if this is too small, network congestion will occur due to large number of tickets
                                            //todo - think about making this variable over time based on trailing ticket claims

#define STAKING_PAYOUT_INTERVAL      10          //modulus to use for making payments of staking rewards
                                                 //mod 0 with LSB of txid means these payouts should be processed


class CStakingPayout {
    std::string address;
    std::string txid;
    int txidLSB;
    uint64_t amount;
};


class CStaking
{

public:
    CStaking();

    //incoming stake command
    void processStakeCommand(std::string address, std::string txid, uint32_t length, char type, uint64_t amount);

    //incoming unstake command (FLEX only)
    void processUnstakeCommand(std::string txid);

    //incoming receipt claim
    void processReceiptClaim();

    //payout processing on current epoch
    void processRewardPayout();

    //my fullnode is entitled to a receipt claim
    void issueReceiptClaim();

private:
    std::mutex payoutLock;
    std::vector<CStakingPayout*> payoutList;
    int lastEpochPaid;
    int payoutIntervalCounter;      //used to spread out payments over many blocks to avoid congestion




    //active staked balances (can be large e.g. > 100,000)


    //receipt claims


    //pending reward payouts

};
