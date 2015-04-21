#pragma once

#include "options.hpp"
#include "PokemonData.hpp"
#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/Layout/Table.h"
#include "Gwen/Controls/TextBox.h"
#include "Gwen/Controls/ImagePanel.h"
#include "Gwen/Controls/GroupBox.h"
#include "Gwen/Controls/CheckBox.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/ScrollControl.h"
#include "Gwen/Controls/HorizontalSlider.h"

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

const char pokedexFont[] = "assets/DroidSansMono.ttf";
const int bigFont = 18;
const int mediumFont = 14;
const int smallFont = 10;

class Pokedex : public Gwen::Controls::Base
{
public:

	Pokedex(Gwen::Controls::Base *pParent, const Gwen::String& name = "");

	void initPokemonList();
	void addRow(const std::string &first, const std::string &second);
	void setPokemon(int id);
	void setMissingNo();
	void filterList(const std::string &query);
    void onRowSelected(Gwen::Controls::Base* pControl);
	void onTypeFilter(Gwen::Controls::Base *pControl);
	void onText(Gwen::Controls::Base *pControl);
	void onPressClear(Gwen::Controls::Base *);
	void onSliderChange(Gwen::Controls::Base *);

private:
	int currentPokemon;

	Gwen::Controls::ImagePanel *imgPanel;
	Gwen::Controls::ListBox *listBox;
	Gwen::Controls::TextBox *textBox;
	Gwen::Controls::GroupBox* groupBox;
	Gwen::Controls::Layout::Table* table;
	Gwen::Controls::Label *flavorLabel;
	Gwen::Controls::TabControl *tabControl;

	Gwen::Font *gwenSmallFont;

	PokemonData pokeData;
	PokemonData::Characteristics characteristics;

	std::vector<Gwen::Controls::HorizontalSlider *> sliders;
	std::vector<Gwen::Controls::CheckBoxWithLabel *> typeCheckBoxes;
	std::vector<int> pokemonFiltered;
};
