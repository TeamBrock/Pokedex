#pragma once

#include <cstddef>
#include <string>
#include <vector>

class PokemonData
{
public:
	struct Characteristics;

	PokemonData();
	~PokemonData();
	PokemonData(const PokemonData&) = delete;
	PokemonData& operator=(const PokemonData&) = delete;

	//set the pokemon you want the getters to work on
	void setPokemon(int ID);
	void setPokemon(const std::string& name);

	int getID();
	std::string getName();
	std::string getSpriteLocation();
	int getBaseHP();
	int getBaseAtt();
	int getBaseDef();
	int getBaseSpAtt();
	int getBaseSpDef();
	int getBaseSpeed();
	std::string getFlavorText();
	double getHeight();
	double getWeight();
	int getTypeID1();
	int getTypeID2();
	//number of pokemon in the database
	size_t numPokemon();

	std::vector<int> getPokemonWithCharacteristics(const Characteristics& characteristics);

	std::string getTypeName(int typeID);
	int getTypeID(const std::string& type);

	void setType(int typeID1, int typeID2 = 0);
	void setType(const std::string& type1, const std::string& type2 = "");

	std::vector<int> getTypesWeakTo();
	std::vector<int> getTypesDoubleWeakTo();
	std::vector<int> getTypesResistantTo();
	std::vector<int> getTypesDoubleResistantTo();
	std::vector<int> getTypesImmuneTo();
	std::vector<int> getTypesDamagedNormallyBy();
 	size_t numTypes();

	//a max characteristic that is lower than corresponding min characteristic will search for pokemon
	//that exactly match the min characteristic instead of searching within a range of values
	//setting a max characteristic to negative will search for pokemon from min to infinity.
	//by default, this structure is initialized to list every pokemon.
	struct Characteristics
	{
		//if true, searches for all pokemon that has every type characteristic you inputed
		//if false, searches for all pokemon that has at least one of the type characteristics you inputed
		bool typesUsingAnd = false;

		int baseHPMin = 0;
		int baseHPMax = -1;
		int baseAttMin = 0;
		int baseAttMax = -1;
		int baseDefMin = 0;
		int baseDefMax = -1;
		int baseSpAttMin = 0;
		int baseSpAttMax = -1;
		int baseSpDefMin = 0;
		int baseSpDefMax = -1;
		int baseSpeedMin = 0;
		int baseSpeedMax = -1;
		double heightMin = 0;
		double heightMax = -1;
		double weightMin = 0;
		double weightMax = -1;

 		//ignored if empty
		std::string nameSubStr;
		//ignored if empty
		std::string nameStartsWith;
		//ignored if empty
		std::vector<int> hasType;
	};

private:
	class PokemonData_;
	PokemonData_* impl;
};
