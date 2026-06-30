#include "CharacterSelectMenuController.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "MainMenuCharacterPreview.h"
#include "PlayerCharacterSelection.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIPanel.h>
#include <RTBEngine/UI/Elements/UIText.h>

#include <cmath>
#include <sstream>
#include <iomanip>

using ThisClass = CharacterSelectMenuController;

namespace {

    RTBEngine::Math::Vector4 kCardNormalBorder(0.42f, 0.44f, 0.50f, 0.85f);
    RTBEngine::Math::Vector4 kCardSelectedBorder(0.91f, 0.66f, 0.29f, 1.0f);
    RTBEngine::Math::Vector4 kCardNormalBackground(0.12f, 0.14f, 0.18f, 0.92f);
    RTBEngine::Math::Vector4 kCardSelectedBackground(0.18f, 0.20f, 0.28f, 0.98f);

    std::string FormatFloat(float value, int precision = 1)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }

}

RTB_REGISTER_COMPONENT(CharacterSelectMenuController)
    RTB_PROPERTY_GAMEOBJECT(characterSelectRoot)
    RTB_PROPERTY_COMPONENT(overlayStatsText, UIText)
    RTB_PROPERTY_COMPONENT(summaryStatsText, UIText)
    RTB_PROPERTY_COMPONENT(summaryNameText, UIText)
    RTB_PROPERTY_COMPONENT(openSelectButton, UIButton)
    RTB_PROPERTY_COMPONENT(backButton, UIButton)
    RTB_PROPERTY_COMPONENT(confirmButton, UIButton)
    RTB_PROPERTY_GAMEOBJECT(characterCard0)
    RTB_PROPERTY_GAMEOBJECT(characterCard1)
    RTB_PROPERTY_GAMEOBJECT(characterCard2)
    RTB_PROPERTY_GAMEOBJECT(characterCard3)
    RTB_PROPERTY_GAMEOBJECT(characterCard4)
    RTB_PROPERTY_COMPONENT(quickSelectButton0, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton1, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton2, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton3, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton4, UIButton)
    RTB_PROPERTY_COMPONENT(characterPreview, MainMenuCharacterPreview)
    RTB_PROPERTY_COMPONENT(clickAudio, AudioSourceComponent)
RTB_END_REGISTER(CharacterSelectMenuController)

void CharacterSelectMenuController::OnStart()
{
    LoadCharacterDefinitions();
    ResolveCharacterCards();
    ResolveQuickSelectButtons();
    BindButtons();

    PlayerCharacterSelection::GetInstance().EnsureSelectionFromCatalog();
    pendingCharacterId = PlayerCharacterSelection::GetInstance().GetSelectedCharacterId();

    SetOverlayVisible(false);
    RefreshSelectionVisuals();
    ApplySummaryFromSelection();
}

void CharacterSelectMenuController::OnDestroy()
{
    callbacksBound = false;
    characterDefinitions.clear();
}

void CharacterSelectMenuController::LoadCharacterDefinitions()
{
    characterDefinitions = CharacterCatalog::GetInstance().GetAllDefinitions();
}

void CharacterSelectMenuController::ResolveCharacterCards()
{
    RTBEngine::ECS::GameObject* cardRoots[kMaxCharacterCards] = {
        characterCard0,
        characterCard1,
        characterCard2,
        characterCard3,
        characterCard4,
    };

    for (int index = 0; index < kMaxCharacterCards; ++index) {
        CharacterCardWidgets& card = characterCards[static_cast<size_t>(index)];
        card.root = cardRoots[index];
        card.frame = nullptr;
        card.button = nullptr;
        card.nameText = nullptr;

        if (!card.root) {
            continue;
        }

        card.frame = card.root->GetComponent<RTBEngine::UI::UIPanel>();
        if (!card.frame) {
            card.frame = card.root->GetComponentInChildren<RTBEngine::UI::UIPanel>();
        }

        card.button = card.root->GetComponent<RTBEngine::UI::UIButton>();
        if (!card.button) {
            card.button = card.root->GetComponentInChildren<RTBEngine::UI::UIButton>();
        }

        card.nameText = FindLabelText(card.root);
    }
}

RTBEngine::UI::UIText* CharacterSelectMenuController::FindLabelText(RTBEngine::ECS::GameObject* cardRoot) const
{
    if (!cardRoot) {
        return nullptr;
    }

    for (RTBEngine::ECS::GameObject* child : cardRoot->GetChildren()) {
        if (!child) {
            continue;
        }

        if (child->GetName() == "NameLabel") {
            if (auto* text = child->GetComponent<RTBEngine::UI::UIText>()) {
                return text;
            }
        }
    }

    return cardRoot->GetComponentInChildren<RTBEngine::UI::UIText>();
}

void CharacterSelectMenuController::ResolveQuickSelectButtons()
{
    RTBEngine::UI::UIButton* buttonRoots[kMaxCharacterCards] = {
        quickSelectButton0,
        quickSelectButton1,
        quickSelectButton2,
        quickSelectButton3,
        quickSelectButton4,
    };

    for (int index = 0; index < kMaxCharacterCards; ++index) {
        QuickSelectWidgets& quickSelect = quickSelectButtons[static_cast<size_t>(index)];
        quickSelect.button = buttonRoots[index];
        quickSelect.frame = nullptr;
        quickSelect.label = nullptr;

        if (!quickSelect.button || !quickSelect.button->GetOwner()) {
            continue;
        }

        RTBEngine::ECS::GameObject* root = quickSelect.button->GetOwner();
        quickSelect.frame = root->GetComponent<RTBEngine::UI::UIPanel>();
        quickSelect.label = root->GetComponentInChildren<RTBEngine::UI::UIText>();
    }
}

void CharacterSelectMenuController::BindButtons()
{
    if (callbacksBound) {
        return;
    }

    if (openSelectButton) {
        openSelectButton->SetOnClick([this]() {
            PlayerCharacterSelection::GetInstance().EnsureSelectionFromCatalog();
            pendingCharacterId = PlayerCharacterSelection::GetInstance().GetSelectedCharacterId();
            SetOverlayVisible(true);
            RefreshSelectionVisuals();
        });
    }

    if (backButton) {
        backButton->SetOnClick([this]() {
            pendingCharacterId = PlayerCharacterSelection::GetInstance().GetSelectedCharacterId();
            SetOverlayVisible(false);
            RefreshSelectionVisuals();
            ApplySummaryFromSelection();
        });
    }

    if (confirmButton) {
        confirmButton->SetOnClick([this]() {
            if (!pendingCharacterId.empty()) {
                ConfirmCharacterSelection(pendingCharacterId);
            }
            SetOverlayVisible(false);
        });
    }

    for (size_t index = 0; index < characterCards.size(); ++index) {
        CharacterCardWidgets& card = characterCards[index];
        if (!card.button) {
            continue;
        }

        CharacterDefinition* definition = nullptr;
        if (index < characterDefinitions.size()) {
            definition = characterDefinitions[index];
        }

        if (definition) {
            if (card.nameText) {
                card.nameText->SetText(definition->displayName);
            }
        } else if (card.nameText) {
            card.nameText->SetText("???");
        }

        const std::string characterId = definition ? definition->characterId : std::string();
        card.button->SetOnClick([this, characterId]() {
            if (!characterId.empty()) {
                SelectPendingCharacter(characterId);
            }
        });
    }

    for (size_t index = 0; index < quickSelectButtons.size(); ++index) {
        QuickSelectWidgets& quickSelect = quickSelectButtons[index];
        if (!quickSelect.button) {
            continue;
        }

        CharacterDefinition* definition = nullptr;
        if (index < characterDefinitions.size()) {
            definition = characterDefinitions[index];
        }

        if (definition) {
            if (quickSelect.label) {
                quickSelect.label->SetText(definition->displayName);
            }
        }

        const std::string characterId = definition ? definition->characterId : std::string();
        quickSelect.button->SetOnClick([this, characterId]() {
            if (!characterId.empty()) {
                ConfirmCharacterSelection(characterId);
            }
        });
    }

    callbacksBound = true;
}

void CharacterSelectMenuController::SetOverlayVisible(bool visible)
{
    if (characterSelectRoot) {
        characterSelectRoot->SetActive(visible);
    }
}

void CharacterSelectMenuController::SelectPendingCharacter(const std::string& characterId)
{
    if (characterId.empty()) {
        return;
    }

    pendingCharacterId = characterId;
    PlayClickSound();
    RefreshSelectionVisuals();
}

void CharacterSelectMenuController::UpdateCharacterPreview()
{
    if (!characterPreview) {
        return;
    }

    std::string previewId = pendingCharacterId;
    if (previewId.empty()) {
        PlayerCharacterSelection& selection = PlayerCharacterSelection::GetInstance();
        selection.EnsureSelectionFromCatalog();
        previewId = selection.GetSelectedCharacterId();
    }

    characterPreview->ShowCharacterById(previewId);
}

void CharacterSelectMenuController::ConfirmCharacterSelection(const std::string& characterId)
{
    if (characterId.empty()) {
        return;
    }

    pendingCharacterId = characterId;
    PlayerCharacterSelection::GetInstance().SetSelectedCharacterId(characterId);
    PlayClickSound();
    RefreshSelectionVisuals();
    ApplySummaryFromSelection();
}

void CharacterSelectMenuController::StyleCardFrame(CharacterCardWidgets& card, bool selected)
{
    if (!card.frame) {
        return;
    }

    card.frame->SetHasBorder(true);
    card.frame->SetBorderThickness(selected ? 3.0f : 1.5f);
    card.frame->SetBorderColor(selected ? kCardSelectedBorder : kCardNormalBorder);
    card.frame->SetBackgroundColor(selected ? kCardSelectedBackground : kCardNormalBackground);

    if (card.nameText) {
        card.nameText->SetColor(selected
            ? RTBEngine::Math::Vector4(1.0f, 0.84f, 0.47f, 1.0f)
            : RTBEngine::Math::Vector4(0.96f, 0.94f, 0.88f, 1.0f));
    }
}

void CharacterSelectMenuController::StyleQuickSelectButton(QuickSelectWidgets& quickSelect, bool selected)
{
    if (quickSelect.frame) {
        quickSelect.frame->SetHasBorder(true);
        quickSelect.frame->SetBorderThickness(selected ? 2.5f : 1.5f);
        quickSelect.frame->SetBorderColor(selected ? kCardSelectedBorder : kCardNormalBorder);
        quickSelect.frame->SetBackgroundColor(selected ? kCardSelectedBackground : kCardNormalBackground);
    }

    if (quickSelect.label) {
        quickSelect.label->SetColor(selected
            ? RTBEngine::Math::Vector4(1.0f, 0.84f, 0.47f, 1.0f)
            : RTBEngine::Math::Vector4(0.96f, 0.94f, 0.88f, 1.0f));
    }
}

void CharacterSelectMenuController::RefreshSelectionVisuals()
{
    for (size_t index = 0; index < characterCards.size(); ++index) {
        CharacterCardWidgets& card = characterCards[index];
        if (!card.root) {
            continue;
        }

        const bool hasDefinition = index < characterDefinitions.size() && characterDefinitions[index];
        const bool selected = hasDefinition
            && characterDefinitions[index]->characterId == pendingCharacterId;
        StyleCardFrame(card, selected);
    }

    for (size_t index = 0; index < quickSelectButtons.size(); ++index) {
        QuickSelectWidgets& quickSelect = quickSelectButtons[index];
        if (!quickSelect.button) {
            continue;
        }

        const bool hasDefinition = index < characterDefinitions.size() && characterDefinitions[index];
        const bool selected = hasDefinition
            && characterDefinitions[index]->characterId == pendingCharacterId;
        StyleQuickSelectButton(quickSelect, selected);
    }

    RefreshStatsTexts();
    UpdateCharacterPreview();
}

CharacterDefinition* CharacterSelectMenuController::FindDefinitionById(const std::string& characterId) const
{
    for (CharacterDefinition* definition : characterDefinitions) {
        if (definition && definition->characterId == characterId) {
            return definition;
        }
    }

    return CharacterCatalog::GetInstance().GetById(characterId);
}

std::string CharacterSelectMenuController::BuildStatsBody(const CharacterDefinition& definition) const
{
    std::ostringstream stream;
    stream << definition.displayName << "\n";
    if (!definition.description.empty()) {
        stream << definition.description << "\n";
    }
    stream << "\n";
    stream << "Health: " << FormatFloat(definition.maxHealth, 0) << "\n";
    stream << "Speed: " << FormatFloat(definition.moveSpeed) << "\n";
    stream << "Sprint: x" << FormatFloat(definition.sprintMultiplier, 2) << "\n";
    stream << "Shots: " << definition.maxShots << "\n";
    stream << "Reload: " << FormatFloat(definition.fullReloadDuration, 1) << " s\n";
    stream << "Damage: " << FormatFloat(definition.projectileDamage, 0) << "\n";
    stream << "Arrow speed: " << FormatFloat(definition.projectileSpeed, 0);
    return stream.str();
}

std::string CharacterSelectMenuController::BuildSummaryBody(const CharacterDefinition& definition) const
{
    std::ostringstream stream;
    stream << "Health: " << FormatFloat(definition.maxHealth, 0) << "\n";
    stream << "Damage: " << FormatFloat(definition.projectileDamage, 0) << "\n";
    stream << "Speed: " << FormatFloat(definition.moveSpeed) << "\n";
    stream << "Sprint: x" << FormatFloat(definition.sprintMultiplier, 2) << "\n";
    stream << "Shots: " << definition.maxShots << "\n";
    stream << "Reload: " << FormatFloat(definition.fullReloadDuration, 1) << " s\n";
    stream << "Arrow speed: " << FormatFloat(definition.projectileSpeed, 0);
    return stream.str();
}

void CharacterSelectMenuController::RefreshStatsTexts()
{
    CharacterDefinition* definition = FindDefinitionById(pendingCharacterId);
    if (!definition) {
        if (overlayStatsText) {
            overlayStatsText->SetText("Select a character.");
        }
        return;
    }

    const std::string statsBody = BuildStatsBody(*definition);
    if (overlayStatsText) {
        overlayStatsText->SetText(statsBody);
    }
}

void CharacterSelectMenuController::PlayClickSound() const
{
    if (clickAudio) {
        clickAudio->PlayOneShot();
    }
}

void CharacterSelectMenuController::ApplySummaryFromSelection()
{
    PlayerCharacterSelection& selection = PlayerCharacterSelection::GetInstance();
    selection.EnsureSelectionFromCatalog();
    CharacterDefinition* definition = selection.GetSelectedDefinition();
    if (!definition) {
        return;
    }

    if (summaryNameText) {
        summaryNameText->SetText("Character: " + definition->displayName);
    }

    if (summaryStatsText) {
        summaryStatsText->SetText(BuildSummaryBody(*definition));
    }
}
