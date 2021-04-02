#include <iostream>
#include <string>
#include "sha256.h"

struct block
{
    int number;
    int nonce;
    std::string data;
    std::string prev_hash;
    std::string this_hash;
    struct block* prev;
    struct block* next;
};

std::string compute_hash(int number, int nonce, std::string data, std::string prev_hash) {
    SHA256 sha256;
    std::string total_string = std::to_string(number) + std::to_string(nonce) + data + prev_hash;
    return sha256(total_string);
}

std::string* do_mining(int hash_difficulty, int number, int nonce, std::string data, std::string prev_hash) {
    std::string* nonce_and_hash = new std::string[2];
    int curr_nonce = nonce;
    std::string curr_hash = compute_hash(number, curr_nonce, data, prev_hash);
    std::string zeros = "";
    for (int i = 0; i < hash_difficulty; i++) {
        zeros = zeros + "0";
    }
    while (curr_hash.substr(0, hash_difficulty) != zeros) {
        curr_nonce++;
        curr_hash = compute_hash(number, curr_nonce, data, prev_hash);
    }
    nonce_and_hash[0] = std::to_string(curr_nonce);
    nonce_and_hash[1] = curr_hash;
    return nonce_and_hash;
}

void update_all_blocks(struct block* blockchain) {
    struct block* curr_block = blockchain;
    while (curr_block->next != NULL) {
        curr_block = curr_block->next;
        curr_block->prev_hash = curr_block->prev->this_hash;
        curr_block->this_hash = compute_hash(curr_block->number, curr_block->nonce, curr_block->data, curr_block->prev_hash);
    }
}

void block_print(struct block* this_block) {
    std::cout << "Block:#" << this_block->number << "\nNonce:" << this_block->nonce << "\nData:" << this_block->data << "\nPrev:" << this_block->prev_hash << "\nHash:" << this_block->this_hash << "\n" << std::endl;
}

void blockchain_print(struct block* this_block) {
    block_print(this_block);
    while (this_block->next != NULL) {
        this_block = this_block->next;
        block_print(this_block);
    }
}

void insert_tail_block(struct block* this_block, int nonce, std::string data) {
    struct block* newblock = new block;
    struct block* tail = this_block;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    struct block* prev_block = tail;
    std::string prev_hash = prev_block->this_hash;
    newblock->number = prev_block->number + 1;
    std::string this_block_hash = compute_hash(newblock->number, nonce, data, prev_hash);
    newblock->nonce = nonce;
    newblock->data = data;
    newblock->prev_hash = prev_hash;
    newblock->this_hash = this_block_hash;
    tail->next = newblock;
    newblock->prev = tail;
    newblock->next = NULL;
}

int main()
{
    std::cout << R"(
  __  __         ____  _            _        _           _       
 |  \/  |       |  _ \| |          | |      | |         (_)      
 | \  / |_   _  | |_) | | ___   ___| | _____| |__   __ _ _ _ __  
 | |\/| | | | | |  _ <| |/ _ \ / __| |/ / __| '_ \ / _` | | '_ \ 
 | |  | | |_| | | |_) | | (_) | (__|   < (__| | | | (_| | | | | |
 |_|  |_|\__, | |____/|_|\___/ \___|_|\_\___|_| |_|\__,_|_|_| |_|
          __/ |                                                  
         |___/                                                      
                                  _                   ___                    _      
                                 | |__  _   _    ___ / _ \ _________   _  __| | ___ 
                                 | '_ \| | | |  / __| | | |_  /_  / | | |/ _` |/ _ \
                                 | |_) | |_| | | (__| |_| |/ / / /| |_| | (_| |  __/
                                 |_.__/ \__, |  \___|\___//___/___|\__, |\__,_|\___|
                                        |___/                      |___/            
)" << '\n';
    int global_hash_difficulty = 3;
    struct block* myblockchain = new block;
    myblockchain->number = 1;
    myblockchain->nonce = 0;
    myblockchain->data = "Genesis block with no transaction data, coinbase initialized...";
    myblockchain->prev_hash = "0000000000000000000000000000000000000000000000000000000000000000";
    myblockchain->next = NULL;
    myblockchain->this_hash = compute_hash(myblockchain->number, myblockchain->nonce, myblockchain->data, myblockchain->prev_hash);
    insert_tail_block(myblockchain, 0, "A paid $100 to B");
    insert_tail_block(myblockchain, 0, "B paid $50 to C");
    insert_tail_block(myblockchain, 0, "C paid $3 to A");
    insert_tail_block(myblockchain, 0, "A paid $20 to X");
    //struct block* second_block = myblockchain->next;
    //block_print(second_block);
    blockchain_print(myblockchain);
    //Mining:
    std::cout << "Start mining...\n\n";
    struct block* curr_block = myblockchain;
    for (int i = 0; i < 5; i++) {
        std::string* nonce_and_hash = do_mining(global_hash_difficulty, curr_block->number, curr_block->nonce, curr_block->data, curr_block->prev_hash);
        curr_block->nonce = std::stoi(nonce_and_hash[0]);
        curr_block->this_hash = nonce_and_hash[1];
        curr_block = curr_block->next;
        update_all_blocks(myblockchain);
    }
    //std::cout << nonce_and_hash[0] << std::endl;
    //std::cout << nonce_and_hash[1] << std::endl;
    std::cout << "After mining:\n\n";
    blockchain_print(myblockchain);
}