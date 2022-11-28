/*
 * SurgeXT for VCV Rack - a Surge Synth Team product
 *
 * Copyright 2019 - 2022, Various authors, as described in the github
 * transaction log.
 *
 * SurgeXT for VCV Rack is released under the Gnu General Public Licence
 * V3 or later (GPL-3.0-or-later). The license is found in the file
 * "LICENSE" in the root of this repository or at
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * All source for Surge XT for VCV Rack is available at
 * https://github.com/surge-synthesizer/surge-rack/
 */

#include "QuadAD.h"
#include "XTModuleWidget.h"
#include "XTWidgets.h"
#include "SurgeXT.h"

namespace sst::surgext_rack::quadad::ui
{
struct QuadADWidget : public widgets::XTModuleWidget
{
    typedef sst::surgext_rack::quadad::QuadAD M;
    QuadADWidget(M *module);

    std::array<std::array<rack::Widget *, M::n_mod_inputs>, M::n_mod_params> overlays;
    std::array<widgets::ModulatableKnob *, M::n_mod_params> underKnobs;
    std::array<widgets::ModToggleButton *, M::n_mod_inputs> toggles;

    void selectModulator(int mod) override
    {
        if (toggles[mod])
            toggles[mod]->onToggle(!toggles[mod]->pressedState);
    }

    void appendModuleSpecificMenu(rack::ui::Menu *menu) override
    {
        if (!module)
            return;
    }
};

QuadADWidget::QuadADWidget(sst::surgext_rack::quadad::ui::QuadADWidget::M *module)
{
    setModule(module);
    typedef layout::LayoutEngine<QuadADWidget, M::ATTACK_0> engine_t;
    engine_t::initializeModulationToBlank(this);

    box.size = rack::Vec(rack::app::RACK_GRID_WIDTH * 12, rack::app::RACK_GRID_HEIGHT);

    auto bg = new widgets::Background(box.size, "QUAD AD", "other", "TotalBlank");
    addChild(bg);
    bg->addAlpha();

    const auto row1 = layout::LayoutConstants::vcoRowCenters_MM[1];
    const auto row2 = layout::LayoutConstants::vcoRowCenters_MM[0];
    const auto row3 = layout::LayoutConstants::vcoRowCenters_MM[0] - (row1 - row2);

    typedef layout::LayoutItem li_t;
    engine_t::layoutItem(this, li_t::createLCDArea(row3 - rack::mm2px(2.5)), "QUAD AD");

    for (int i = 0; i < M::n_ads; ++i)
    {
        const auto col = layout::LayoutConstants::firstColumnCenter_MM +
                         i * layout::LayoutConstants::columnWidth_MM;
        // fixme use the enums
        // clang-format off
        std::vector<li_t> layout = {
          {li_t::KNOB9, "ATTACK", M::ATTACK_0 + i, col, row3},
          {li_t::KNOB9, "DECAY", M::DECAY_0 + i, col, row2},

          {li_t::PORT, "TRIG", M::TRIGGER_0 + i, col, row1},
        };
        // clang-format on

        for (const auto &lay : layout)
        {
            engine_t::layoutItem(this, lay, "QuadVCA");
        }

        auto lcdw = rack::app::RACK_GRID_WIDTH * 12 - widgets::LCDBackground::posx * 2;
        auto w = lcdw / 4.0;

        auto yAD = rack::mm2px(row3) - rack::mm2px(13);
        auto x = widgets::LCDBackground::posx + w * i;
        auto h = rack::mm2px(5.5);
        auto mode = widgets::PlotAreaToggleClick::create(rack::Vec(x, yAD), rack::Vec(w, h), module,
                                                         M::MODE_0 + i);
        mode->align = widgets::PlotAreaToggleClick::CENTER;
        addChild(mode);
    }

    engine_t::addModulationSection(this, M::n_mod_inputs, M::MOD_INPUT_0);

    int kc = 0;
    for (int i = M::OUTPUT_0; i < M::OUTPUT_0 + M::n_ads; ++i)
    {
        auto yc = layout::LayoutConstants::inputRowCenter_MM;
        auto xc = layout::LayoutConstants::firstColumnCenter_MM +
                  layout::LayoutConstants::columnWidth_MM * kc;

        addOutput(
            rack::createOutputCentered<widgets::Port>(rack::mm2px(rack::Vec(xc, yc)), module, i));

        auto bl = layout::LayoutConstants::inputLabelBaseline_MM;
        auto lab = engine_t::makeLabelAt(bl, kc, "ENV" + std::to_string(i - M::OUTPUT_0 + 1),
                                         style::XTStyle::TEXT_LABEL_OUTPUT);
        addChild(lab);
        kc++;
    }
    resetStyleCouplingToModule();
}
} // namespace sst::surgext_rack::quadad::ui
// namespace sst::surgext_rack::quadad::ui

rack::Model *modelQuadAD =
    rack::createModel<sst::surgext_rack::quadad::ui::QuadADWidget::M,
                      sst::surgext_rack::quadad::ui::QuadADWidget>("SurgeXTQuadAD");
