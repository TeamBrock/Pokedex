#include <iostream>
#include <iomanip>
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
		auto clearButton = new Gwen::Controls::Button(advancedPage);
		clearButton->SetPos(0, 0);
		clearButton->SetSize(40, 20);
		clearButton->SetText("Clear");
		clearButton->SetFont(pokedexFont, smallFont, false);
		clearButton->onPress.Add(this, &Pokedex::onPressClear);

		const int initialY = clearButton->Height() + clearButton->GetPos().y + 5;

		int x = 0;
		int y = initialY;

		for (size_t i = 1; i <= pokeData.numTypes(); ++i) {
			auto check = new Gwen::Controls::CheckBoxWithLabel(advancedPage);
			typeCheckBoxes.push_back(check);
			check->SetPos(x, y);
			std::string name = pokeData.getTypeName(i);
			check->Label()->SetText(name);
			check->Label()->SetFont(pokedexFont, smallFont, false);
			check->Checkbox()->onCheckChanged.Add(this, &Pokedex::onTypeFilter);
			check->Checkbox()->SetName(name);
			y += check->Height() + 2;
			if (y > (180 + initialY)) {
				y = initialY;
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
		groupBox->SetSize(WINDOW_WIDTH - tabControl->Width() - 35, statsTab->Height() - flavorLabel->Height() - 20);
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
	if (pokemonFiltered.size()) {
		currentPokemon = pokemonFiltered[0];
		pokeData.setPokemon(currentPokemon);

		for (const auto &id : pokemonFiltered) {
			pokeData.setPokemon(id);
			std::string name = pokeData.getName();
			listBox->AddItem(name, name);
		}
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

	label1->SetWrap(true);
	label1->SetFont(pokedexFont, smallFont, false);
	label2->SetFont(pokedexFont, smallFont, false);
	label2->SetWrap(true);
}

template <typename T>
std::string toStringWithPrecision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out << std::setprecision(n) << a_value;
	return out.str();
}

void Pokedex::setPokemon(int id)
{
	pokeData.setPokemon(id);
	if (pokeData.getTypesWeakTo().size()) {
		std::cout << "has types weak to\n";
	}

	imgPanel->SetImage(pokeData.getSpriteLocation());
	flavorLabel->SetText(pokeData.getFlavorText());
	table->Clear();

	addRow("Height", toStringWithPrecision(pokeData.getHeight(), 6));
	addRow("Weight", toStringWithPrecision(pokeData.getWeight(), 6));

	addRow("Base HP", std::to_string(pokeData.getBaseHP()));
	addRow("Base Attk", std::to_string(pokeData.getBaseAtt()));
	addRow("Base Sp. Attk", std::to_string(pokeData.getBaseSpAtt()));
	addRow("Base Def", std::to_string(pokeData.getBaseDef()));
	addRow("Base Sp. Def", std::to_string(pokeData.getBaseSpDef()));
	addRow("Base Sp. Def", std::to_string(pokeData.getBaseSpDef()));

	std::vector<int> typesWeakTo = pokeData.getTypesWeakTo();
	std::vector<int> typesDoubleWeakTo = pokeData.getTypesDoubleWeakTo();
	std::vector<int> typesResistantTo  = pokeData.getTypesResistantTo();
	std::vector<int> typesDoubleResistantTo = pokeData.getTypesDoubleResistantTo();
	std::vector<int> typesImmuneTo = pokeData.getTypesImmuneTo();
	std::vector<int> typesDamagedNormallyBy = pokeData.getTypesDamagedNormallyBy();

	std::string strWeakTo;
	std::string strDoubleWeakTo;
	std::string strResistantTo;
	std::string strDoubleResistantTo;
	std::string strImmuneTo;
	std::string strDamagedNormallyBy;

	auto processType = [](std::string &str, std::vector<int> &types, PokemonData &data) {
		size_t length = types.size();
		size_t index = 0;
		for (auto type : types) {
			str.append(data.getTypeName(type));
			if (index != length - 1) {
				str.append(", ");
			}
			++index;
		}
	};

	processType(strWeakTo, typesWeakTo, pokeData);
	processType(strDoubleWeakTo, typesDoubleWeakTo, pokeData);
	processType(strResistantTo, typesResistantTo, pokeData);
	processType(strDoubleResistantTo, typesDoubleResistantTo, pokeData);
	processType(strImmuneTo, typesImmuneTo, pokeData);
	processType(strDamagedNormallyBy, typesDamagedNormallyBy, pokeData);

	if (strWeakTo.size()) {
		addRow("Types weak to", strWeakTo);
	}

	if (strDoubleWeakTo.size()) {
		addRow("Types double weak to", strDoubleWeakTo);
	}

	if (strResistantTo.size()) {
		addRow("Types resistant to", strResistantTo);
	}

	if (strDoubleResistantTo.size()) {
		addRow("Types double resistant to", strDoubleResistantTo);
	}

	if (strImmuneTo.size()) {
		addRow("Types immune to", strImmuneTo);
	}

	if (strDamagedNormallyBy.size()) {
		addRow("Types damaged normally by", strDamagedNormallyBy);
	}
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
	setPokemon(currentPokemon);
	//listBox->SelectByString(pokeData.getName());
}

void Pokedex::onPressClear(Gwen::Controls::Base *)
{
	for (auto check : typeCheckBoxes) {
		check->Checkbox()->SetChecked(false);
	}
}
