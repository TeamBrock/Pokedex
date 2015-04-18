#include <iostream>
#include "pokedex.hpp"
#include "Gwen/Controls/CollapsibleList.h"

Pokedex::Pokedex(Gwen::Controls::Base *pParent, const Gwen::String& name)
		: Gwen::Controls::Base(pParent, name)
{
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	{
		tabControl = new Gwen::Controls::TabControl(this);
		int y = 80;
		tabControl->SetBounds(5, y, WINDOW_WIDTH/4, WINDOW_HEIGHT - y - 5);
	}

	Gwen::Controls::TabButton* basicButton = tabControl->AddPage("Basic");
	Gwen::Controls::TabButton* advancedButton = tabControl->AddPage("Advanced");

	Gwen::Controls::Base* basicPage = basicButton->GetPage();
	Gwen::Controls::Base* advancedPage = advancedButton->GetPage();

	{
		textBox = new Gwen::Controls::TextBox(basicPage);
		textBox->SetPos(5, 17);
		textBox->SetSize(WINDOW_WIDTH/4 - 25, 40);
		textBox->SetText("");
		textBox->onTextChanged.Add(this, &Pokedex::onText);
	}

	{
		listBox = new Gwen::Controls::ListBox(basicPage);
		listBox->SetPos(textBox->GetPos().x, textBox->GetPos().y + textBox->Height() + 20);
		listBox->SetSize(WINDOW_WIDTH/4 - 25, tabControl->Height() - 150);

		initPokemonList();

		listBox->SelectByString(pokeData.getName());
		listBox->onRowSelected.Add(this, &Pokedex::onRowSelected);
	}

	{
		int x = 0;
		int y = 0;

		for (size_t i = 1; i <= pokeData.numTypes(); ++i) {
			auto check = new Gwen::Controls::CheckBoxWithLabel(advancedPage);
			check->SetPos(x, y);
			std::string name = pokeData.getTypeName(i);
			check->Label()->SetText(name);
			check->Label()->SetFont(pokedexFont, smallFont, false);
			check->Checkbox()->onCheckChanged.Add(this, &Pokedex::onTypeFilter);
			check->Checkbox()->SetName(name);
			check->Checkbox()->SetChecked(true);
			y += check->Height() + 2;
			if (y + check->Height() + 2 > options::WINDOW_HEIGHT) {
				y = listBox->GetPos().y + listBox->Height() + 10;
				x += 90;
			}
		}
	}

	{
		imgPanel = new Gwen::Controls::ImagePanel(this);
		imgPanel->SetBounds((WINDOW_WIDTH - tabControl->Width())/2 + 100, 0, 200, 200);
	}

	auto statsTab = new Gwen::Controls::TabControl(this);
	statsTab->SetSize(WINDOW_WIDTH - tabControl->Width() - 15, WINDOW_HEIGHT - imgPanel->Height() - 10);
	statsTab->SetPos(tabControl->GetPos().x + tabControl->Width() + 5, imgPanel->GetPos().y + imgPanel->Height() + 5);

	{
		flavorLabel = new Gwen::Controls::Label(statsTab);
		flavorLabel->SetWrap(true);
		flavorLabel->SetFont(pokedexFont, mediumFont, false);
		flavorLabel->SetPos(5, 5);
		flavorLabel->SetSize(statsTab->Width() - 10, 60);
	}

	{
		groupBox = new Gwen::Controls::GroupBox(statsTab);
		groupBox->SetPos(10, flavorLabel->Height() + flavorLabel->GetPos().y);
		groupBox->SetSize(WINDOW_WIDTH - tabControl->Width() - 35, statsTab->Height() - flavorLabel->Height() - 15);
		groupBox->SetFont(pokedexFont, mediumFont, false);
		groupBox->SetText("Stats");
		groupBox->SetShouldDrawBackground(true);

		table = new Gwen::Controls::Layout::Table(groupBox);
		table->Dock(Gwen::Pos::Fill);
		table->SetColumnCount(2);
	}

	setPokemon(currentPokemon);
}

void Pokedex::initPokemonList()
{
	pokemonFiltered = pokeData.getPokemonWithCharacteristics(characteristics);
	currentPokemon = pokemonFiltered[0];
	pokeData.setPokemon(currentPokemon);

	for (const auto &id : pokemonFiltered) {
		pokeData.setPokemon(id);
		listBox->AddItem(pokeData.getName());
	}
}

void Pokedex::addRow(const std::string &first, const std::string &second)
{
	auto row = new Gwen::Controls::Layout::TableRow(groupBox);
	row->SetColumnCount(2);
	row->SetCellText(0, first);
	row->SetCellText(1, second);
	table->AddRow(row);

	auto label1 = row->GetCellContents(0);
	auto label2 = row->GetCellContents(1);

	label1->SetFont(pokedexFont, mediumFont, false);
	label2->SetFont(pokedexFont, mediumFont, false);
}

void Pokedex::setPokemon(int id)
{
	pokeData.setPokemon(id);
	imgPanel->SetImage(pokeData.getSpriteLocation());
	flavorLabel->SetText(pokeData.getFlavorText());
	table->Clear();
	addRow("Base HP", std::to_string(pokeData.getBaseHP()));
	addRow("Base Attk", std::to_string(pokeData.getBaseAtt()));
	addRow("Base Sp. Attk", std::to_string(pokeData.getBaseSpAtt()));
	addRow("Base Def", std::to_string(pokeData.getBaseDef()));
	addRow("Base Sp. Def", std::to_string(pokeData.getBaseSpDef()));
}

void Pokedex::onRowSelected(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::ListBox* ctrl = (Gwen::Controls::ListBox*)pControl;
	std::string name = ctrl->GetSelectedRow()->GetText(0);;

	for (const auto &id : pokemonFiltered) {
		pokeData.setPokemon(id);
		if (pokeData.getName() == name) {
			currentPokemon = pokeData.getID();
			break;
		}
	}

	setPokemon(currentPokemon);
}

void Pokedex::onText(Gwen::Controls::Base *pControl)
{
	Gwen::Controls::TextBox *ctrl = static_cast<Gwen::Controls::TextBox *>(pControl);
	listBox->Clear();
	filterList(ctrl->GetText());
}

void Pokedex::onTypeFilter(Gwen::Controls::Base *pControl)
{
	auto ctrl = static_cast<Gwen::Controls::CheckBox*>(pControl);
	listBox->Clear();
	if (ctrl->IsChecked()) {
		characteristics.hasType.push_back(pokeData.getTypeID(ctrl->GetName()));
	} else {
		characteristics.hasType.erase(
			std::remove(characteristics.hasType.begin(),
						characteristics.hasType.end(),pokeData.getTypeID(ctrl->GetName())),
			characteristics.hasType.end()
		);
	}
	filterList(textBox->GetText());
}

void Pokedex::filterList(const std::string &query)
{
	characteristics.nameStartsWith = query;
	initPokemonList();
}
