#include <string>
#include <iostream>
#include <regex>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "options.hpp"
#include "render_context.hpp"
#include "imgui_sdlbackend.hpp"
#include "pokedex_screen.hpp"
#include "text.hpp"

#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/Layout/Table.h"
#include "Gwen/Controls/TextBox.h"
#include "Gwen/Controls/ImagePanel.h"
#include "Gwen/Controls/GroupBox.h"

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

struct PokemonIndex {
	PokemonIndex(PokemonData &pokeData)
	{
		name = pokeData.getName();
		imagePath = pokeData.getSpriteLocation();
		id = pokeData.getID();
		baseHP = pokeData.getBaseHP();
		baseAtt = pokeData.getBaseAtt();
		baseDef = pokeData.getBaseDef();
		baseSpAtt = pokeData.getBaseSpAtt();
		baseSpDef = pokeData.getBaseSpDef();
		flavorText = pokeData.getFlavorText();
		type = pokeData.getTypeName(pokeData.getTypeID1());
		if (pokeData.getTypeID2() != 0) {
			type.append(", " + pokeData.getTypeName(pokeData.getTypeID2()));
		}
		weakTo = pokeData.getTypesWeakTo();
		doubleWeakTo = pokeData.getTypesDoubleWeakTo();
		resistantTo = pokeData.getTypesResistantTo();
		doubleResistantTo = pokeData.getTypesDoubleResistantTo();
		immuneTo = pokeData.getTypesImmuneTo();
		normalTo = pokeData.getTypesDamagedNormallyBy();
	}

	Gwen::Controls::Layout::TableRow* tableRow;

	int id;
	int baseHP;
	int baseAtt;
	int baseDef;
	int baseSpAtt;
	int baseSpDef;
	int textDistance;

	std::string name;
	std::string imagePath;
	std::string flavorText;
	std::string type;

	std::vector<int> weakTo;
	std::vector<int> doubleWeakTo;
	std::vector<int> resistantTo;
	std::vector<int> doubleResistantTo;
	std::vector<int> immuneTo;
	std::vector<int> normalTo;
};

class Pokedex : public Gwen::Controls::Base
{
public:

    Pokedex(Gwen::Controls::Base *pParent, const Gwen::String& name = "")
		: Gwen::Controls::Base(pParent, name)
    {
		SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);

		{
			textBox = new Gwen::Controls::TextBox(this);
			textBox->SetPos(5, 70);
			textBox->SetSize(WINDOW_WIDTH/4, 40);
			textBox->SetText("");
			textBox->onTextChanged.Add(this, &Pokedex::onText);
		}

		{
			listBox = new Gwen::Controls::ListBox(this);
			listBox->SetPos(textBox->GetPos().x, textBox->GetPos().y + textBox->Height() + 25);
			listBox->SetSize(WINDOW_WIDTH/4, WINDOW_HEIGHT-150-5);
			initPokemonList();
			listBox->SelectByString(pokemonList[0].name);
            listBox->onRowSelected.Add(this, &Pokedex::rowSelected);
		}

		{
			imgPanel= new Gwen::Controls::ImagePanel(this);
            imgPanel->SetBounds((WINDOW_WIDTH - listBox->GetPos().x + listBox->Width())/2, 0, 200, 200);
		}

		{
			auto flavorGroup = new Gwen::Controls::GroupBox(this);
			flavorGroup->SetSize(imgPanel->GetPos().x - 5 - (textBox->GetPos().x + textBox->Width()),
								175);
			flavorGroup->SetPos(textBox->GetPos().x + textBox->Width() + 5, 10);
			flavorGroup->SetText("Description");
			flavorGroup->SetFont("assets/DroidSansMono.ttf", 14, false);

			flavorLabel = new Gwen::Controls::Label(flavorGroup);
			flavorLabel->Dock(Gwen::Pos::Fill);
			flavorLabel->SetWrap(true);
			flavorLabel->SetFont("assets/DroidSansMono.ttf", 14, false);
		}

		{
			groupBox = new Gwen::Controls::GroupBox(this);
			groupBox->SetSize(WINDOW_WIDTH - listBox->Width() - 15, WINDOW_HEIGHT - imgPanel->Height() - 10);
			groupBox->SetPos(listBox->GetPos().x + listBox->Width() + 5,
						     imgPanel->GetPos().y + imgPanel->Height() + 5);
			groupBox->SetFont("assets/DroidSansMono.ttf", 14, false);
			groupBox->SetText("Stats");

			table = new Gwen::Controls::Layout::Table(groupBox);
			table->Dock(Gwen::Pos::Fill);
			table->SetColumnCount(2);
		}

		setPokemon(1);
    }

	void initPokemonList()
	{
		for (int i = 1, totalPokemon = pokeData.numPokemon(); i <= totalPokemon; ++i) {
			pokeData.setPokemon(i);
			pokemonList.push_back(PokemonIndex(pokeData));
			pokemonList[i-1].tableRow = listBox->AddItem(pokemonList[i-1].name);
		}
	}

	void addPokemonToList(int id)
	{
		pokemonList[id].tableRow = listBox->AddItem(pokemonList[id].name);
	}

	void addRow(const std::string &first, const std::string &second)
	{
		auto row = new Gwen::Controls::Layout::TableRow(groupBox);
		row->SetColumnCount(2);
		row->SetCellText(0, first);
		row->SetCellText(1, second);
		table->AddRow(row);

		auto label1 = row->GetCellContents(0);
		auto label2 = row->GetCellContents(1);

		label1->SetFont("assets/DroidSansMono.ttf", 14, false);
		label2->SetFont("assets/DroidSansMono.ttf", 14, false);
	}

	void setPokemon(int id)
	{
		PokemonIndex &index = pokemonList[id - 1];
		imgPanel->SetImage(index.imagePath);
		flavorLabel->SetText(index.flavorText);

		table->Clear();
		addRow("Base HP", std::to_string(index.baseHP));
		addRow("Base Attk", std::to_string(index.baseAtt));
		addRow("Base Sp. Attk", std::to_string(index.baseSpAtt));
		addRow("Base Def", std::to_string(index.baseDef));
		addRow("Base Sp. Def", std::to_string(index.baseSpDef));
	}

    void rowSelected(Gwen::Controls::Base* pControl)
    {
        Gwen::Controls::ListBox* ctrl = (Gwen::Controls::ListBox*)pControl;
		std::string name = ctrl->GetSelectedRow()->GetText(0);;
		int id = 1;

		for (auto &pokemon : pokemonList) {
			if (pokemon.name == name) {
				id = pokemon.id;
				break;
			}
		}

		setPokemon(id);
	}

	void onText(Gwen::Controls::Base *pControl)
	{
		Gwen::Controls::TextBox *ctrl = static_cast<Gwen::Controls::TextBox *>(pControl);
		listBox->Clear();
		std::vector<PokemonIndex *> matches;
		std::string query = ctrl->GetText();
		if (query.size() == 0) {
			// add back all entries
			for (int i = 0; i < pokemonList.size(); ++i) {
				addPokemonToList(i);
			}
		} else { // valid query
			std::regex self_regex(query, std::regex_constants::ECMAScript | std::regex_constants::icase);
			for (auto &index : pokemonList) {
				if (std::regex_search(index.name, self_regex)) {
					matches.push_back(&index);
				}
			}
			// add matches
			for (auto index : matches) {
				addPokemonToList(index->id - 1);
			}
		}
	}

	Gwen::Controls::ImagePanel *imgPanel;
	Gwen::Controls::ListBox *listBox;
	Gwen::Controls::TextBox *textBox;
	Gwen::Controls::GroupBox* groupBox;
	Gwen::Controls::Layout::Table* table;
	Gwen::Controls::Label *flavorLabel;

	PokemonData pokeData;
	std::vector<PokemonIndex> pokemonList;
};

bool PokedexScreen::initialize(RenderContext *context, ScreenDispatcher *dispatcher)
{
	if (!Screen::initialize(context, dispatcher)) {
		return false;
	}

	m_pokeData.setPokemon(1);
	m_currentPokemonID = m_pokeData.getID();
	setPokedexData(m_currentPokemonID);

    m_gwenRenderer = new Gwen::Renderer::SDL2(m_context->window, m_context->renderer);
	m_gwenSkin = new Gwen::Skin::TexturedBase(m_gwenRenderer);
    m_gwenSkin->SetRender(m_gwenRenderer);
    m_gwenSkin->Init("assets/DefaultSkin.png");
    m_gwenSkin->SetDefaultFont("assets/DroidSansMono.ttf", 20);
    
    m_gwenCanvas = new Gwen::Controls::Canvas(m_gwenSkin);
    m_gwenCanvas->SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_gwenCanvas->SetDrawBackground(true);
    m_gwenCanvas->SetBackgroundColor(Gwen::Color(150, 170, 170, 255));

    // Create our unittest control (which is a Window with controls in it)
    m_pokedexBase = new Pokedex(m_gwenCanvas);
    m_gwenInput.Initialize(m_gwenCanvas);

	return true;
}

void PokedexScreen::setPokedexData(int id)
{
	m_pokeData.setPokemon(id);
	m_pokeData.setType(m_pokeData.getTypeID1(), m_pokeData.getTypeID2());
}

void PokedexScreen::handleEvent(const SDL_Event &sdlEvent)
{
	switch(sdlEvent.type) {
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
			}
			break;
	}

	m_gwenInput.ProcessEvent(const_cast<SDL_Event*>(&sdlEvent));
}

PokedexScreen::~PokedexScreen()
{
	delete m_gwenCanvas;
	delete m_gwenSkin;
	delete m_gwenRenderer;
}

void PokedexScreen::frameStep(unsigned long)
{
	SDL_RenderClear(m_context->renderer);

	m_gwenRenderer->BeginContext(NULL);
	m_gwenCanvas->RenderCanvas();

	SDL_RenderPresent(m_context->renderer);
}
