#include "includes.hpp"
#include "ui/record_layer.hpp"
#include "ui/game_ui.hpp"
#include "ui/clickbot_layer.hpp"
#include "ui/macro_editor.hpp"
#include "ui/render_settings_layer.hpp"
#include "hacks/layout_mode.hpp"
#include "hacks/show_trajectory.hpp"

#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/SettingV3.hpp>

static void onKeybind(bool down, bool repeat, std::string_view id) {
#ifdef GEODE_IS_WINDOWS
    auto& g = Global::get();

    if (!down || (repeat && id != "keybind_step_frame") ||
        (LevelEditorLayer::get() && !g.mod->getSettingValue<bool>("editor_keybinds")) ||
        g.mod->getSettingValue<bool>("disable_keybinds")) {
        return;
    }

    if (g.state != state::recording && g.mod->getSettingValue<bool>("recording_only_keybinds"))
        return;

    if (id == "keybind_open_menu") {
        if (g.layer) {
            static_cast<RecordLayer*>(g.layer)->onClose(nullptr);
            return;
        }
        RecordLayer::openMenu();
    }
    else if (id == "keybind_toggle_recording") {
        Macro::toggleRecording();
    }
    else if (id == "keybind_toggle_playing") {
        Macro::togglePlaying();
    }
    else if (id == "keybind_toggle_frame_stepper" && PlayLayer::get()) {
        Global::toggleFrameStepper();
    }
    else if (id == "keybind_step_frame") {
        Global::frameStep();
    }
    else if (id == "keybind_toggle_speedhack") {
        Global::toggleSpeedhack();
    }
    else if (id == "keybind_show_trajectory") {
        g.mod->setSavedValue("macro_show_trajectory", !g.mod->getSavedValue<bool>("macro_show_trajectory"));
        if (g.layer) {
            if (auto* toggle = static_cast<RecordLayer*>(g.layer)->trajectoryToggle)
                toggle->toggle(g.mod->getSavedValue<bool>("macro_show_trajectory"));
        }
        g.showTrajectory = g.mod->getSavedValue<bool>("macro_show_trajectory");
        if (!g.showTrajectory)
            ShowTrajectory::trajectoryOff();
    }
    else if (id == "keybind_toggle_render" && PlayLayer::get()) {
        bool result = Renderer::toggle();
        if (result && g.renderer.recording)
            Notification::create("Started Rendering", NotificationIcon::Info)->show();
        if (g.layer) {
            if (auto* toggle = static_cast<RecordLayer*>(g.layer)->renderToggle)
                toggle->toggle(g.renderer.recording);
        }
    }
    else if (id == "keybind_toggle_noclip") {
        g.mod->setSavedValue("macro_noclip", !g.mod->getSavedValue<bool>("macro_noclip"));
        if (g.layer) {
            if (auto* toggle = static_cast<RecordLayer*>(g.layer)->noclipToggle)
                toggle->toggle(g.mod->getSavedValue<bool>("macro_noclip"));
        }
    }
#endif
}

$on_game(Loaded) {
    // Track the six native Geometry Dash gameplay bindings, including user remaps.
    const std::array<std::pair<std::string_view, size_t>, 6> gameBinds = {{
        {"jump-p1", 0},
        {"move-left-p1", 1},
        {"move-right-p1", 2},
        {"jump-p2", 3},
        {"move-left-p2", 4},
        {"move-right-p2", 5}
    }};

    for (auto const& [setting, index] : gameBinds) {
        KeybindSettingPressedEventV3("robtop.geometry-dash", std::string(setting))
            .listen([index](Keybind const&, bool down, bool repeat, double) {
                if (!repeat)
                    Global::get().heldButtons[index] = down;
            }).leak();
    }

#ifdef GEODE_IS_WINDOWS
    listenForKeybindSettingPresses("keybind_open_menu", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_open_menu");
    });
    listenForKeybindSettingPresses("keybind_toggle_recording", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_toggle_recording");
    });
    listenForKeybindSettingPresses("keybind_toggle_playing", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_toggle_playing");
    });
    listenForKeybindSettingPresses("keybind_toggle_speedhack", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_toggle_speedhack");
    });
    listenForKeybindSettingPresses("keybind_toggle_noclip", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_toggle_noclip");
    });
    listenForKeybindSettingPresses("keybind_toggle_frame_stepper", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_toggle_frame_stepper");
    });
    listenForKeybindSettingPresses("keybind_step_frame", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_step_frame");
    });
    listenForKeybindSettingPresses("keybind_show_trajectory", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_show_trajectory");
    });
    listenForKeybindSettingPresses("keybind_toggle_render", [](Keybind const&, bool down, bool repeat, double) {
        onKeybind(down, repeat, "keybind_toggle_render");
    });
#endif
}
