#ifndef Hector_test_Canvas_h
#define Hector_test_Canvas_h

#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TGraph.h"
#include "TGraphErrors.h"

#include <string.h>

#define font_type(x) 130 + x

namespace Hector {
  class Canvas : public TCanvas {
  public:
    class PaveText : public TPaveText {
    public:
      inline PaveText(float x1, float y1, float x2, float y2, const char* text = "")
          : TPaveText(x1, y1, x2, y2, "NDC") {
        TPaveText::SetTextAlign(13);
        if (strcmp(text, "") != 0) {
          TString txt = text;
          if (txt.Contains("@@")) {
            TObjArray* tok = txt.Tokenize("@@");
            for (int i = 0; i < tok->GetEntries(); i++)
              TPaveText::AddText(dynamic_cast<TObjString*>(tok->At(i))->String());
          } else
            TPaveText::AddText(text);
        }
        TPaveText::SetFillColor(0);
        TPaveText::SetFillStyle(0);
        TPaveText::SetLineColor(0);
        TPaveText::SetLineWidth(0);
        TPaveText::SetShadowColor(0);
        TPaveText::SetTextAlign(kHAlignLeft + kVAlignTop);
        TPaveText::SetTextFont(font_type(2));
        TPaveText::SetTextSize(0.058);
      }
    };

  public:
    inline Canvas(const char* name, const char* title = "", bool ratio = false, bool draw_logo = true)
        :  //TCanvas( name, "", 450, 450 ),
          TCanvas(name, "", 800, 800),
          fTitle(title),
          fLegX1(0.5),
          fLegY1(0.75),
          fRatio(ratio),
          fDrawLogo(draw_logo) {
      Build();
    }

    inline void SetSize(float size = 600) { TCanvas::SetCanvasSize(size, 600); }

    inline void Prettify(TH1* obj) {
      TAxis *x = dynamic_cast<TAxis*>(obj->GetXaxis()), *y = dynamic_cast<TAxis*>(obj->GetYaxis()),
            *z = dynamic_cast<TAxis*>(obj->GetZaxis());
      x->SetLabelFont(font_type(2));
      x->SetLabelSize(0.038);
      x->SetTitleFont(font_type(2));
      x->SetTitleSize(0.05);
      y->SetLabelFont(font_type(2));
      y->SetLabelSize(0.038);
      y->SetTitleFont(font_type(2));
      y->SetTitleSize(0.05);
      z->SetLabelFont(font_type(2));
      z->SetLabelSize(0.038);
      z->SetTitleFont(font_type(2));
      z->SetTitleSize(0.05);
      y->SetTitleOffset(1.4);
      if (fRatio) {
        x->SetTitleOffset(1.4);
        x->SetTickLength(0.05);
        y->SetTickLength(0.03);
      }
      // axis titles
      TString ttle = obj->GetTitle();
      if (ttle.Contains("@@")) {
        TObjArray* tok = ttle.Tokenize("@@");
        TString x_title, y_title, unit, form_spec, distrib;
        if (tok->GetEntries() > 0)
          x_title = dynamic_cast<TObjString*>(tok->At(0))->String();
        if (tok->GetEntries() > 1)
          y_title = dynamic_cast<TObjString*>(tok->At(1))->String();
        if (tok->GetEntries() > 2) {
          unit = ((TObjString*)tok->At(2))->String();
          if (unit.Contains("?")) {  // extract format specifier
            TObjArray* tok2 = unit.Tokenize("?");
            if (tok2->GetEntries() > 1) {
              unit = dynamic_cast<TObjString*>(tok2->At(0))->String();
              form_spec = dynamic_cast<TObjString*>(tok2->At(1))->String();
            } else {
              unit = "";
              form_spec = dynamic_cast<TObjString*>(tok2->At(0))->String();
            }
          }
        }
        if (tok->GetEntries() > 3) {
          distrib = ((TObjString*)tok->At(3))->String();
        }
        if (!unit.IsNull() || !form_spec.IsNull()) {
          if (!unit.IsNull())
            x_title = Form("%s (%s)", x_title.Data(), unit.Data());
          if (!distrib.IsNull()) {
            if (!form_spec.IsNull()) {
              TString format = Form("%%s (%s / %%%s %%s)", distrib.Data(), form_spec.Data());
              y_title = Form(format.Data(), y_title.Data(), GetBinning(obj), unit.Data());
            } else
              y_title = Form("%s (%s / %d %s)",
                             y_title.Data(),
                             distrib.Data(),
                             static_cast<unsigned int>(GetBinning(obj)),
                             unit.Data());
          } else {
            if (!form_spec.IsNull()) {
              TString format = Form("%%s / %%%s %%s", form_spec.Data());
              y_title = Form(format.Data(), y_title.Data(), GetBinning(obj), unit.Data());
            } else
              y_title = Form("%s / %d %s", y_title.Data(), static_cast<unsigned int>(GetBinning(obj)), unit.Data());
          }
        }
        obj->GetXaxis()->SetTitle(x_title);
        obj->GetYaxis()->SetTitle(y_title);
        obj->SetTitle("");
      } else {
        obj->GetXaxis()->SetTitle(ttle);
        obj->SetTitle("");
      }
    }

    inline void DrawDiagonal(const TH1* obj) {
      TLine l;
      l.SetLineWidth(2);
      l.SetLineColor(kGray);
      l.SetLineStyle(2);
      l.DrawLine(obj->GetXaxis()->GetXmin(),
                 obj->GetYaxis()->GetXmin(),
                 obj->GetXaxis()->GetXmax(),
                 obj->GetYaxis()->GetXmax());
    }

    inline void SetTopLabel(const char* lab = "") {
      TCanvas::cd();
      if (strcmp(lab, "") != 0)
        fTitle = lab;
      if (!fTopLabel)
        BuildTopLabel();
      else
        fTopLabel->Clear();
      fTopLabel->AddText(fTitle);
    }

    inline void SetLegendX1(double x) { fLegX1 = x; }
    inline void SetLegendY1(double y) { fLegY1 = y; }
    inline void AddLegendEntry(const TObject* obj, const char* title, Option_t* option = "lpf") {
      if (!fLeg)
        BuildLegend();
      fLeg->AddEntry(obj, title, option);
      const unsigned int num_entries = fLeg->GetNRows();
      if (num_entries > 3) {
        fLeg->SetY1(fLeg->GetY1() - (num_entries - 3) * 0.025);
      }
    }

    inline void Save(const char* ext, const char* out_dir = ".") {
      if (strstr(ext, "pdf") == nullptr && strstr(ext, "png") == nullptr && strstr(ext, "C") == nullptr)
        return;
      TCanvas::cd();
      if (fLeg)
        fLeg->Draw();
      if (fTopLogo)
        fTopLogo->Draw();
      if (fTopLabel)
        fTopLabel->Draw();
      TCanvas::SaveAs(::Form("%s/%s.%s", out_dir, TCanvas::GetName(), ext));
    }

  private:
    inline void Build() {
      TCanvas::SetLeftMargin(0.14);
      TCanvas::SetTopMargin(0.06);
      TCanvas::SetRightMargin(0.1);
      TCanvas::SetBottomMargin(0.15);
      TCanvas::SetTicks(1, 1);

      TCanvas::cd();
      if (fDrawLogo) {
        fTopLogo = std::make_unique<PaveText>(0.14, 0.95, 0.2, 1.01);
        fTopLogo->SetTextSize(0.04);
        fTopLogo->SetTextFont(font_type(2));
        fTopLogo->SetTextAlign(kHAlignLeft + kVAlignTop);
        fTopLogo->AddText("#font[32]{Hector}");
        fTopLogo->AddText("#scale[0.7]{simulation}");
      }
      SetTopLabel();
      if (fRatio)
        DivideCanvas();
    }

    inline void DivideCanvas() {
      TCanvas::Divide(1, 2);
      TPad *p1 = dynamic_cast<TPad*>(TCanvas::GetPad(1)), *p2 = dynamic_cast<TPad*>(TCanvas::GetPad(2));
      p1->SetPad(0., 0.5, 1., 1.);
      p2->SetPad(0., 0.0, 1., 0.5);
      p1->SetLeftMargin(TCanvas::GetLeftMargin());
      p1->SetRightMargin(TCanvas::GetRightMargin());
      p2->SetLeftMargin(TCanvas::GetLeftMargin());
      p2->SetRightMargin(TCanvas::GetRightMargin());
      p1->SetBottomMargin(0.07);
      p2->SetTopMargin(0.07);
      p1->SetTopMargin(TCanvas::GetTopMargin() + 0.08);
      p2->SetBottomMargin(TCanvas::GetBottomMargin() + 0.03);
      p1->SetTicks(1, 1);
      p2->SetTicks(1, 1);
      TCanvas::cd(1);
    }

    inline void BuildTopLabel() {
      TCanvas::cd();
      fTopLabel = std::make_unique<PaveText>(0.5, 0.945, 0.925, 0.96);
      fTopLabel->SetTextSize(0.035);
      fTopLabel->SetTextAlign(kHAlignRight + kVAlignBottom);
    }

    inline void BuildLegend() {
      if (fLeg)
        return;
      if (fRatio)
        TCanvas::cd(1);
      fLeg = std::make_unique<TLegend>(fLegX1, fLegY1, fLegX1 + 0.3, fLegY1 + 0.15);
      fLeg->SetLineColor(kWhite);
      fLeg->SetLineWidth(0);
      fLeg->SetFillStyle(0);
      fLeg->SetTextFont(font_type(2));
      fLeg->SetTextSize(0.035);
    }
    inline float GetBinning(const TH1* h) {
      if (!h)
        return -1.;
      return (h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin()) / h->GetXaxis()->GetNbins();
    }

    TString fTitle;
    std::unique_ptr<PaveText> fTopLabel, fTopLogo;
    std::unique_ptr<TLegend> fLeg;
    double fLegX1, fLegY1;
    bool fRatio, fDrawLogo;
  };
}  // namespace Hector

#endif
