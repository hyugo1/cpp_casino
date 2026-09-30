#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <random>
#include <vector>
#include <memory>
#include <array>

#include "utils/utils.h"
#include "player/player.h"
#include "game/game.h"
#include "card/card.h"
#include "games/coinflip.h"
#include "games/guessthecard.h"
#include "games/blackjack.h"

int main() { 
    Player player{};
    
    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    player.setName(name);

    std::cout << "Hello, " << player.getName() << "!\n";
    std::cout << "Your current balance is: " << player.getBalance() << '\n';
    std::cout << "Your current win rate is: " << player.getWinRatio() << "%\n";
    
    std::vector<std::unique_ptr<CasinoGame>> games;
    games.push_back(std::make_unique<CoinFlip>());
    games.push_back(std::make_unique<BlackJack>());
    games.push_back(std::make_unique<GuessTheCard>());
    
    
    while (true) {
        std::cout << "What would you like to play?\n";
        std::cout << "---------------------------------------\n";
        
        std::cout << "0: Quit\n";
        for (size_t i = 0; i < games.size(); ++i) {
            std::cout <<  i+1 << ": " << games[i]->getGameName() << '\n';
        }

        int selection {};
        if (!Utils::tryRead(selection)) { continue; }

        if (selection == 0) {
            std::cout << "Thanks for playing!\n";
            return 0;
        }

        if (selection < 1 || selection > static_cast<int>(games.size())) {
            std::cout << "Not a valid entry. Try again!\n";
            continue;
        }

        games[selection - 1]->play(player);
        std::cout << "Your current balance is: " << player.getBalance() << '\n';
        std::cout << "Your current win rate is: " << player.getWinRatio() << "%\n";
        std::cout << '\n';
    }

    return 0;
}