// player.cpp
#include <iostream>
#include <string>
#include <vector>

#include "player.h"

Player::Player(std::string name, double bal, int games_played, int wins)
    : m_name{name}, m_bal{bal}, m_total_games_played{games_played}, m_wins{wins} {}

const std::string_view Player::getName() const { return m_name; }
void Player::setName(const std::string& name) {m_name = name;}
double Player::getBalance() const { return m_bal; }
double Player::getWinRatio() const { 
    if (m_total_games_played == 0) return 0.0;
    return (static_cast<double>(m_wins) / m_total_games_played) * 100;
}

void Player::addToBalance(double amount) { m_bal += amount; }
void Player::subFromBalance(double amount) { m_bal -= amount; }
void Player::recordResult(bool won) {
    if (won) { ++m_wins; }
    else { ++m_losses; }
    ++m_total_games_played;
}

void Player::askDifficulty() {
    int choice {};
    do {
        std::cout << "What difficulty would you like?\n";
        std::cout << "1: Easy 2: Medium or 3: Hard? Enter a number: ";
        if (!Utils::tryRead(choice)) {continue; }
    } while (choice < 1 || choice > 3);
    m_difficulty = choice;
    std::cout << "Chose: " << difficultyLevel[m_difficulty - 1] << ".\n";
}

int Player::getDifficulty() const {
    return m_difficulty;
}