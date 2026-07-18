#pragma once

#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <array>
#include <string>
#include <vector>

namespace RTBEngine {
    namespace Scene {
        class AudioSourceComponent;
    }
    namespace UI {
        class UIButton;
        class UIPanel;
        class UIText;
    }
}

class CharacterDefinition;
class MainMenuCharacterPreview;

class CharacterSelectMenuController : public RTBEngine::Scene::Component {
public:
    static constexpr int kMaxCharacterCards = 5;

    CharacterSelectMenuController() = default;
    ~CharacterSelectMenuController() override = default;

    RTBEngine::Scene::GameObject* characterSelectRoot = nullptr;
    RTBEngine::UI::UIText* overlayStatsText = nullptr;
    RTBEngine::UI::UIText* summaryStatsText = nullptr;
    RTBEngine::UI::UIText* summaryNameText = nullptr;
    RTBEngine::UI::UIButton* openSelectButton = nullptr;
    RTBEngine::UI::UIButton* backButton = nullptr;
    RTBEngine::UI::UIButton* confirmButton = nullptr;

    RTBEngine::Scene::GameObject* characterCard0 = nullptr;
    RTBEngine::Scene::GameObject* characterCard1 = nullptr;
    RTBEngine::Scene::GameObject* characterCard2 = nullptr;
    RTBEngine::Scene::GameObject* characterCard3 = nullptr;
    RTBEngine::Scene::GameObject* characterCard4 = nullptr;

    RTBEngine::UI::UIPanel* characterCardFrame0 = nullptr;
    RTBEngine::UI::UIPanel* characterCardFrame1 = nullptr;
    RTBEngine::UI::UIPanel* characterCardFrame2 = nullptr;
    RTBEngine::UI::UIPanel* characterCardFrame3 = nullptr;
    RTBEngine::UI::UIPanel* characterCardFrame4 = nullptr;

    RTBEngine::UI::UIButton* characterCardButton0 = nullptr;
    RTBEngine::UI::UIButton* characterCardButton1 = nullptr;
    RTBEngine::UI::UIButton* characterCardButton2 = nullptr;
    RTBEngine::UI::UIButton* characterCardButton3 = nullptr;
    RTBEngine::UI::UIButton* characterCardButton4 = nullptr;

    RTBEngine::UI::UIText* characterCardNameText0 = nullptr;
    RTBEngine::UI::UIText* characterCardNameText1 = nullptr;
    RTBEngine::UI::UIText* characterCardNameText2 = nullptr;
    RTBEngine::UI::UIText* characterCardNameText3 = nullptr;
    RTBEngine::UI::UIText* characterCardNameText4 = nullptr;

    RTBEngine::UI::UIButton* quickSelectButton0 = nullptr;
    RTBEngine::UI::UIButton* quickSelectButton1 = nullptr;
    RTBEngine::UI::UIButton* quickSelectButton2 = nullptr;
    RTBEngine::UI::UIButton* quickSelectButton3 = nullptr;
    RTBEngine::UI::UIButton* quickSelectButton4 = nullptr;

    RTBEngine::UI::UIPanel* quickSelectFrame0 = nullptr;
    RTBEngine::UI::UIPanel* quickSelectFrame1 = nullptr;
    RTBEngine::UI::UIPanel* quickSelectFrame2 = nullptr;
    RTBEngine::UI::UIPanel* quickSelectFrame3 = nullptr;
    RTBEngine::UI::UIPanel* quickSelectFrame4 = nullptr;

    RTBEngine::UI::UIText* quickSelectLabel0 = nullptr;
    RTBEngine::UI::UIText* quickSelectLabel1 = nullptr;
    RTBEngine::UI::UIText* quickSelectLabel2 = nullptr;
    RTBEngine::UI::UIText* quickSelectLabel3 = nullptr;
    RTBEngine::UI::UIText* quickSelectLabel4 = nullptr;

    MainMenuCharacterPreview* characterPreview = nullptr;
    RTBEngine::Scene::AudioSourceComponent* clickAudio = nullptr;

    RTB_COMPONENT(CharacterSelectMenuController)

    void OnStart() override;
    void OnDestroy() override;

private:
    struct CharacterCardWidgets {
        RTBEngine::Scene::GameObject* root = nullptr;
        RTBEngine::UI::UIPanel* frame = nullptr;
        RTBEngine::UI::UIButton* button = nullptr;
        RTBEngine::UI::UIText* nameText = nullptr;
    };

    struct QuickSelectWidgets {
        RTBEngine::UI::UIButton* button = nullptr;
        RTBEngine::UI::UIPanel* frame = nullptr;
        RTBEngine::UI::UIText* label = nullptr;
    };

    std::array<CharacterCardWidgets, kMaxCharacterCards> characterCards{};
    std::array<QuickSelectWidgets, kMaxCharacterCards> quickSelectButtons{};
    std::vector<CharacterDefinition*> characterDefinitions;
    std::string pendingCharacterId;
    bool callbacksBound = false;

    void BindButtons();
    void BindCharacterCardWidgets();
    void BindQuickSelectWidgets();
    void LoadCharacterDefinitions();
    void SetOverlayVisible(bool visible);
    void SelectPendingCharacter(const std::string& characterId);
    void ConfirmCharacterSelection(const std::string& characterId);
    void RefreshSelectionVisuals();
    void RefreshStatsTexts();
    void ApplySummaryFromSelection();
    void UpdateCharacterPreview();
    std::string BuildStatsBody(const CharacterDefinition& definition) const;
    std::string BuildSummaryBody(const CharacterDefinition& definition) const;
    void StyleCardFrame(CharacterCardWidgets& card, bool selected);
    void StyleQuickSelectButton(QuickSelectWidgets& quickSelect, bool selected);
    void PlayClickSound() const;
    CharacterDefinition* FindDefinitionById(const std::string& characterId) const;
};
