#include <iostream>
#include <iomanip>
#include "pokedex.hpp"
#include "Gwen/Controls/CollapsibleList.h"

template <typename T>
std::string toStringWithPrecision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out << std::setprecision(n) << a_value;
	return out.str();
}

class SliderWithLabel : public Gwen::Controls::Base
{
public:
	GWEN_CONTROL_INLINE(SliderWithLabel, Base)
	{
		SetSize(200, 30);
		m_Slider = new Gwen::Controls::HorizontalSlider(this);
		m_Slider->Dock(Gwen::Pos::Left);
		m_Slider->SetMargin(Gwen::Margin(0, 2, 2, 2));
		m_Slider->SetTabable(false);
		m_Slider->onValueChanged.Add(this, &SliderWithLabel::onSliderChange);

		m_Label = new Gwen::Controls::Label(this);
		m_Label->Dock(Gwen::Pos::Fill);
		m_Label->SetTabable(false);
		SetTabable(false);
	}

	Gwen::Controls::Label *GetLabel() { return m_Label; }
	Gwen::Controls::HorizontalSlider *GetSlider() { return m_Slider; }

	void onSliderChange(Gwen::Controls::Base *ctrl)
	{
		auto slider = static_cast<Gwen::Controls::HorizontalSlider *>(ctrl);
		m_Label->SetText(toStringWithPrecision(slider->GetFloatValue()));
	}

private:
	Gwen::Controls::Label *m_Label;
	Gwen::Controls::HorizontalSlider *m_Slider;
};

Pokedex::Pokedex(Gwen::Controls::Base *pParent, const Gwen::String& name)
		: Gwen::Controls::Base(pParent, name)
{
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	{
		tabControl = new Gwen::Controls::TabControl(this);
		int y = 80;
		tabControl->SetBounds(5, y, WINDOW_WIDTH/3.8, WINDOW_HEIGHT - y - 5);
	}

	Gwen::Controls::TabButton* basicButton = tabControl->AddPage("Basic");
	Gwen::Controls::TabButton* advancedButton = tabControl->AddPage("Advanced");
	Gwen::Controls::Base* basicPage = basicButton->GetPage();
	Gwen::Controls::Base* advancedPage = advancedButton->GetPage();

	Gwen::Controls::ScrollControl* advScroll = new Gwen::Controls::ScrollControl(advancedPage);
	advScroll->Dock(Gwen::Pos::Fill);

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
		auto clearButton = new Gwen::Controls::Button(advScroll);
		clearButton->SetPos(0, 0);
		clearButton->SetSize(40, 20);
		clearButton->SetText("Reset");
		clearButton->SetFont(pokedexFont, smallFont, false);
		clearButton->onPress.Add(this, &Pokedex::onPressClear);

		const int initialY = clearButton->Height() + clearButton->GetPos().y + 5;

		int x = 0;
		int y = initialY;

		for (size_t i = 1; i <= pokeData.numTypes(); ++i) {
			auto check = new Gwen::Controls::CheckBoxWithLabel(advScroll);
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

		int sliderY = 200 + 30;
		sliders.reserve(16);

		auto createSlider = [&](const std::string &titleStr,
								float min,
								float max,
								float defaultValue)
		{
			auto title = new Gwen::Controls::Label(advScroll);
			title->SetPos(0, sliderY);
			title->SetFont(pokedexFont, smallFont, false);
			title->SetText(titleStr);
			title->SetSize(100, 18);
			sliderY += title->Height() - 5;

			auto sliderWithLabel = new SliderWithLabel(advScroll);
			auto slider = sliderWithLabel->GetSlider();

			sliderWithLabel->SetPos(0, sliderY);
			sliderWithLabel->GetLabel()->SetFont(pokedexFont, smallFont, false);

			slider->SetSize(150, 20);
			slider->SetRange(min, max + 10);
			slider->SetFloatValue(defaultValue);
			slider->SetNotchCount(max);
			slider->SetName(titleStr);

			sliderY += slider->Height() + 10;
			sliders.push_back(sliderWithLabel->GetSlider());
		};

		createSlider("Min. HP", 0, 250, 10);
		createSlider("Max. HP", 0, 250, 250);

		createSlider("Min. Attk", 5, 134, 5);
		createSlider("Max. Attk", 5, 134, 134);

		createSlider("Min. Def", 5, 180, 5);
		createSlider("Max. Def", 5, 180, 180);

		createSlider("Min. Sp. Attk", 15, 154, 15);
		createSlider("Max. Sp. Attk", 15, 154, 154);

		createSlider("Min. Sp. Def", 20, 125, 20);
		createSlider("Max. Sp. Def", 20, 125, 125);

		createSlider("Min. Speed", 15, 140, 15);
		createSlider("Max. Speed", 15, 140, 140);

		createSlider("Min. Height", 0.2, 8.8, 0.2);
		createSlider("Max. Height", 0.2, 8.8, 8.8);

		createSlider("Min. Weight", 0.1, 460, 0.1);
		createSlider("Max. Weight", 0.1, 460, 460);

		for (auto slider : sliders) {
			slider->onValueChanged.Add(this, &Pokedex::onSliderChange);
		}
	}
		
	{
		imgPanel = new Gwen::Controls::ImagePanel(this);
		imgPanel->SetBounds((WINDOW_WIDTH - tabControl->Width())/2 + 100, 0, 200, 200);
	}

	auto statsTab = new Gwen::Controls::TabControl(this);
	statsTab->SetSize(WINDOW_WIDTH - tabControl->Width() - 15,
			WINDOW_HEIGHT - imgPanel->Height() - 10);
	statsTab->SetPos(tabControl->GetPos().x + tabControl->Width() + 5,
			imgPanel->GetPos().y + imgPanel->Height() + 5);

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

void Pokedex::setPokemon(int id)
{
	pokeData.setPokemon(id);
	pokeData.setType(pokeData.getTypeID1(), pokeData.getTypeID2());

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
		addRow("Weak to", strWeakTo);
	}

	if (strDoubleWeakTo.size()) {
		addRow("Double weak to", "Test");
	}

	if (strResistantTo.size()) {
		addRow("Resistant to", strResistantTo);
	}

	if (strDoubleResistantTo.size()) {
		addRow("Double resistant to", strDoubleResistantTo);
	}

	if (strImmuneTo.size()) {
		addRow("Immune to", strImmuneTo);
	}

	if (strDamagedNormallyBy.size()) {
		addRow("Damaged normally by", strDamagedNormallyBy);
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
}

void Pokedex::onPressClear(Gwen::Controls::Base *)
{
	for (auto check : typeCheckBoxes) {
		check->Checkbox()->SetChecked(false);
	}

	for (auto slider : sliders) {
		std::string name = slider->GetName();

		if (name == "Min. HP") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. HP") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Attk") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Attk") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Def") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Def") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Sp. Attk") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Sp. Attk") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Sp. Def") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Sp. Def") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Speed") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Speed") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Height") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Height") {
			slider->SetFloatValue(slider->GetMax());
		} else if (name == "Min. Weight") {
			slider->SetFloatValue(slider->GetMin());
		} else if (name == "Max. Weight") {
			slider->SetFloatValue(slider->GetMax());
		}
	}
}

void Pokedex::onSliderChange(Gwen::Controls::Base *ctrl)
{
	auto slider = static_cast<Gwen::Controls::Slider *>(ctrl);
	std::string name = ctrl->GetName();
	if (name == "Min. HP") {
		characteristics.baseHPMin = slider->GetFloatValue();
	} else if (name == "Max. HP") {
		characteristics.baseHPMax = slider->GetFloatValue();
	} else if (name == "Min. Attk") {
		characteristics.baseAttMin = slider->GetFloatValue();
	} else if (name == "Max. Attk") {
		characteristics.baseAttMax = slider->GetFloatValue();
	} else if (name == "Min. Def") {
		characteristics.baseDefMin= slider->GetFloatValue();
	} else if (name == "Max. Def") {
		characteristics.baseDefMax = slider->GetFloatValue();
	} else if (name == "Min. Sp. Attk") {
		characteristics.baseSpAttMin = slider->GetFloatValue();
	} else if (name == "Max. Sp. Attk") {
		characteristics.baseSpAttMax = slider->GetFloatValue();
	} else if (name == "Min. Sp. Def") {
		characteristics.baseSpDefMin = slider->GetFloatValue();
	} else if (name == "Max. Sp. Def") {
		characteristics.baseSpDefMax = slider->GetFloatValue();
	} else if (name == "Min. Speed") {
		characteristics.baseSpeedMin= slider->GetFloatValue();
	} else if (name == "Max. Speed") {
		characteristics.baseSpeedMax = slider->GetFloatValue();
	} else if (name == "Min. Height") {
		characteristics.heightMin = slider->GetFloatValue();
	} else if (name == "Max. Height") {
		characteristics.heightMax = slider->GetFloatValue();
	} else if (name == "Min. Weight") {
		characteristics.weightMin = slider->GetFloatValue();
	} else if (name == "Max. Weight") {
		characteristics.weightMax = slider->GetFloatValue();
	}

	listBox->Clear();
	characteristics.nameStartsWith = textBox->GetText();
	initPokemonList();
	setPokemon(currentPokemon);
}
