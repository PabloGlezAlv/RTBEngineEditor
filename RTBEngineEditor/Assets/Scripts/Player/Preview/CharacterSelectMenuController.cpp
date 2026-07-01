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

    float ResolveDisplayedDamage(const CharacterDefinition& definition, bool isMelee)
    {
        if (!isMelee || definition.meleeTickCount <= 1) {
            return definition.projectileDamage;
        }

        return definition.projectileDamage / static_cast<float>(definition.meleeTickCount);
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
    RTB_PROPERTY_COMPONENT(characterCardFrame0, UIPanel)
    RTB_PROPERTY_COMPONENT(characterCardFrame1, UIPanel)
    RTB_PROPERTY_COMPONENT(characterCardFrame2, UIPanel)
    RTB_PROPERTY_COMPONENT(characterCardFrame3, UIPanel)
    RTB_PROPERTY_COMPONENT(characterCardFrame4, UIPanel)
    RTB_PROPERTY_COMPONENT(characterCardButton0, UIButton)
    RTB_PROPERTY_COMPONENT(characterCardButton1, UIButton)
    RTB_PROPERTY_COMPONENT(characterCardButton2, UIButton)
    RTB_PROPERTY_COMPONENT(characterCardButton3, UIButton)
    RTB_PROPERTY_COMPONENT(characterCardButton4, UIButton)
    RTB_PROPERTY_COMPONENT(characterCardNameText0, UIText)
    RTB_PROPERTY_COMPONENT(characterCardNameText1, UIText)
    RTB_PROPERTY_COMPONENT(characterCardNameText2, UIText)
    RTB_PROPERTY_COMPONENT(characterCardNameText3, UIText)
    RTB_PROPERTY_COMPONENT(characterCardNameText4, UIText)
    RTB_PROPERTY_COMPONENT(quickSelectButton0, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton1, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton2, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton3, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectButton4, UIButton)
    RTB_PROPERTY_COMPONENT(quickSelectFrame0, UIPanel)
    RTB_PROPERTY_COMPONENT(quickSelectFrame1, UIPanel)
    RTB_PROPERTY_COMPONENT(quickSelectFrame2, UIPanel)
    RTB_PROPERTY_COMPONENT(quickSelectFrame3, UIPanel)
    RTB_PROPERTY_COMPONENT(quickSelectFrame4, UIPanel)
    RTB_PROPERTY_COMPONENT(quickSelectLabel0, UIText)
    RTB_PROPERTY_COMPONENT(quickSelectLabel1, UIText)
    RTB_PROPERTY_COMPONENT(quickSelectLabel2, UIText)
    RTB_PROPERTY_COMPONENT(quickSelectLabel3, UIText)
    RTB_PROPERTY_COMPONENT(quickSelectLabel4, UIText)
    RTB_PROPERTY_COMPONENT(characterPreview, MainMenuCharacterPreview)
    RTB_PROPERTY_COMPONENT(clickAudio, AudioSourceComponent)
RTB_END_REGISTER(CharacterSelectMenuController)

void CharacterSelectMenuController::OnStart()
{
    LoadCharacterDefinitions();
    BindCharacterCardWidgets();
    BindQuickSelectWidgets();
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

void CharacterSelectMenuController::BindCharacterCardWidgets()
{
    RTBEngine::ECS::GameObject* cardRoots[kMaxCharacterCards] = {
        characterCard0,
        characterCard1,
        characterCard2,
        characterCard3,
        characterCard4,
    };
    RTBEngine::UI::UIPanel* cardFrames[kMaxCharacterCards] = {
        characterCardFrame0,
        characterCardFrame1,
        characterCardFrame2,
        characterCardFrame3,
        characterCardFrame4,
    };
    RTBEngine::UI::UIButton* cardButtons[kMaxCharacterCards] = {
        characterCardButton0,
        characterCardButton1,
        characterCardButton2,
        characterCardButton3,
        characterCardButton4,
    };
    RTBEngine::UI::UIText* cardNameTexts[kMaxCharacterCards] = {
        characterCardNameText0,
        characterCardNameText1,
        characterCardNameText2,
        characterCardNameText3,
        characterCardNameText4,
    };

    for (int index = 0; index < kMaxCharacterCards; ++index) {
        CharacterCardWidgets& card = characterCards[static_cast<size_t>(index)];
        card.root = cardRoots[index];
        card.frame = cardFrames[index];
        card.button = cardButtons[index];
        card.nameText = cardNameTexts[index];
    }
}

void CharacterSelectMenuController::BindQuickSelectWidgets()
{
    RTBEngine::UI::UIButton* buttonRoots[kMaxCharacterCards] = {
        quickSelectButton0,
        quickSelectButton1,
        quickSelectButton2,
        quickSelectButton3,
        quickSelectButton4,
    };
    RTBEngine::UI::UIPanel* buttonFrames[kMaxCharacterCards] = {
        quickSelectFrame0,
        quickSelectFrame1,
        quickSelectFrame2,
        quickSelectFrame3,
        quickSelectFrame4,
    };
    RTBEngine::UI::UIText* buttonLabels[kMaxCharacterCards] = {
        quickSelectLabel0,
        quickSelectLabel1,
        quickSelectLabel2,
        quickSelectLabel3,
        quickSelectLabel4,
    };

    for (int index = 0; index < kMaxCharacterCards; ++index) {
        QuickSelectWidgets& quickSelect = quickSelectButtons[static_cast<size_t>(index)];
        quickSelect.button = buttonRoots[index];
        quickSelect.frame = buttonFrames[index];
        quickSelect.label = buttonLabels[index];
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
    const bool isMelee = definition.meleeRange > 0.0f;

    std::ostringstream stream;
    stream << definition.displayName << "\n";
    if (!definition.description.empty()) {
        stream << definition.description << "\n";
    }
    stream << "\n";
    stream << "Health: " << FormatFloat(definition.maxHealth, 0) << "\n";
    stream << "Speed: " << FormatFloat(definition.moveSpeed) << "\n";
    stream << "Sprint: x" << FormatFloat(definition.sprintMultiplier, 2) << "\n";
    stream << (isMelee ? "Charges: " : "Shots: ") << definition.maxShots << "\n";
    stream << "Reload: " << FormatFloat(definition.fullReloadDuration, 1) << " s\n";
    stream << (isMelee ? "Damage (per tick): " : "Damage: ")
           << FormatFloat(ResolveDisplayedDamage(definition, isMelee), 0);
    if (isMelee) {
        stream << "\nRange: " << FormatFloat(definition.meleeRange, 1);
    } else if (!definition.projectilePrefabRef.empty()) {
        stream << "\nProjectile speed: " << FormatFloat(definition.projectileSpeed, 0);
    }
    return stream.str();
}

std::string CharacterSelectMenuController::BuildSummaryBody(const CharacterDefinition& definition) const
{
    const bool isMelee = definition.meleeRange > 0.0f;

    std::ostringstream stream;
    stream << "Health: " << FormatFloat(definition.maxHealth, 0) << "\n";
    stream << (isMelee ? "Damage (per tick): " : "Damage: ")
           << FormatFloat(ResolveDisplayedDamage(definition, isMelee), 0) << "\n";
    stream << "Speed: " << FormatFloat(definition.moveSpeed) << "\n";
    stream << "Sprint: x" << FormatFloat(definition.sprintMultiplier, 2) << "\n";
    stream << (isMelee ? "Charges: " : "Shots: ") << definition.maxShots << "\n";
    stream << "Reload: " << FormatFloat(definition.fullReloadDuration, 1) << " s";
    if (isMelee) {
        stream << "\nRange: " << FormatFloat(definition.meleeRange, 1);
    } else if (!definition.projectilePrefabRef.empty()) {
        stream << "\nProjectile speed: " << FormatFloat(definition.projectileSpeed, 0);
    }
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
