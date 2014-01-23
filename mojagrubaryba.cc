#include "mojagrubaryba.h"
#include <iostream>

/* MojaGrubaRyba */

void MojaGrubaRyba::setDie(std::shared_ptr<Die> die)
{
	this->die.reset(die);
}

void MojaGrubaRyba::play(unsigned int rounds)
{
	playersInGame = players.size();
	for(int r = 1; r <= rounds && playersInGame > 1; r++)
	{
		std::cout << "Runda " << r << '\n';
		for(Player &player : players)
		{
			std::cout << player.getName();
			if(player.inGame() == false)
			{
				std::cout << " *** bankrut ***\n";
				continue;
			}
			if(player.getWaitingTime() == 0)
			{
				int moves = 0;
				moves += die.roll();
				moves += die.roll();
				for(int m = 1; m < moves; m++)
				{
					int filedNum = (player.getPos() + m) % board.getSize();
					board.field(fieldNum).pass(player);
				}
				int newPos = (player.getPos() + moves) % board.getSize();
				player.setPos(newPos);
				board.field(newPos).action(player);
				std::cout << " pole: " << field(newPos).getName() << " gotowka: " << player.getFishcoins() << '\n';
			}
			else
			{
				std::cout << " pole: " << field(player.getPos()).getName << " *** czekanie: " << player.getWaitingTime() << " ***\n";
				player.setWaitingTime(player.getWaitingTime()-1);
			}
		}
	}
}

MojaGrubaRyba::MojaGrubaRyba()
{
	//TODO chyba trzeba przerobic na factory jakies 
	vector < Field > fields;
	fields.push_back(new StartField);
	fields.push_back(new PropertyField("Anemonia", 160));
	fields.push_back(new IslandField);
	fields.push_back(new PropertyField("Aporina", 220));
	fields.push_back(new AquariumField(3));
	fields.push_back(new PropertyField("Hotel w grocie", 300));
	fields.push_back(new PropertyField("Menella", 280));
	fields.push_back(new DepositField("Laguna", 15));
	fields.push_back(new PropertyField("Statek", 250));
	fields.push_back(new PrizeField("Blazenki", 120));
	fields.push_back(new PropertyField("Pennatula", 400));
	fields.push_back(new FineField("Rekin", 180));
	board.fields = std::move(fields);
}

/* Player */

int Player::getPos()
{
	return pos;
}

void Player::setPos(int newPos)
{
	pos = newPos;
}

bool DumbComputerPlayer::wantBuy(std::string const& propertyName)
{
	static int it = 1;
	if(it == 3)
	{
		it = 1;
		return true;
	}
	it++;
	return false;
}

bool DumbComputerPlayer::wantSell()
{
	return false;
}

bool SmartassComputerPlayer::wantBuy(std::string const& propertyName)
{
	return true;
}

bool HumanPlayer::wantBuy(std::string const& propertyName)
{
	return humanInterface.wantBuy(propertyName);
}

bool HumanPlayer::wantSell(std::string const& propertyName)
{
	return humanInterface.wantSell(propertyName);
}

/* Field */

const std::string Field::getName()
{
	return this->name;
}

StartField::StartField(int cash) : cash(cash), name("Start")
{

}

void StartField::action(shared_ptr< Player > p)
{
	p->addFishcoins(this->cash);
}

void StartField::pass(shared_ptr< Player > p)
{
	p->addFishcoins(this->cash);
}

PrizeField::PrizeField(std::string name, int ammount) : name(name), ammount(ammount)
{

}

void PrizeField::action(shared_ptr<Player> p)
{
	p->addFishcoins(this->ammount);
}

void PrizeField::pass(shared_ptr<Player> p)
{
	//nothing
}


IslandField::IslandField() : name("Wyspa")
{

}


void IslandField::action(shared_ptr<Player> p)
{
	//nothing
}

void IslandField::pass(shared_ptr<Player> p)
{
	//nothing
}


PropertyField::PropertyField(std::string name, int price, int percent)
	: name(name), price(price), percent(percent)
{

}

void PropertyField::action(shared_ptr<Player> p)
{
	if (this->owner.expired()){//noninitialized
		if(p->wantBuy(this->name)){
			this->owner = p;
		}
	} else if (auto owner = this->owner.lock()){
		if (owner != p) {
			int fare = (this->getPercent * this->price) / 100;
			p->pay(fare);
			owner->addFishcoins(fare);
		}
	}
	else {
		throw deletedPlayerException;//wysraj wyjatek
	}
}

void PropertyField::pass(shared_ptr<Player> p)
{

}

int PropertyField::getPrice(){
	return this->price;
}

void PropertyField::sold(){
	this->owner.reset();
}

PublicPropertyField::PublicPropertyField(std::string name, int price)
	: PropertyField(name, price, 40)
{

}

CoralPropertyField::CoralPropertyField(std::string name, int price)
	: PropertyField(name, price, 20)
{

}

DepositField::DepositField(std::string name, int ammount) : name(name), ammount(ammount)
{
	
}
void DepositField::action(shared_ptr<Player> p)
{
	p->addFishcoins(this->fishcoins);
	this->fishcoins = 0;
}

void DepositField::pass(shared_ptr<Player> p)
{
	this->fishcoins += p->pay(this->ammount);
}

FineField::FineField(std::string name. int ammount) : name(name), ammount(ammount)
{

}
void FineField::action(shared_ptr<Player> p)
{
	this->fishcoins += p->pay(this->ammount);
}

void FineField::pass(shared_ptr<Player> p)
{

}

AquariumField::AquariumField(int turns) : turnsToWait(turns)
{

}
void AquariumField::action(shared_ptr<Player> p)
{
	p->setWaitingTime(this->turnsToWait);
}

void AquariumField::pass(shared_ptr<Player> p)
{

}

