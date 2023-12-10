#### code to draw plots comparing muon distributions from high pt and medium.
#### The plots use LUXE dump geometry

import os, sys, time
import argparse
from ROOT import *
from copy import copy, deepcopy
from collections import OrderedDict


### normalize a hist (1D or 2D)
def normalizeHist(hist):
   strType = str(type(hist))
   ### normalize 1D hist
   if 'TH1' in strType:
      w = hist.Integral(0, hist.GetNbinsX()+1)
      if w!=0:
        hist.Scale(1./w)
      else:
        print("can't normalize, it's an empty histogram")

   ### normaize 2D hist
   else:
      w = hist.Integral(0, hist.GetNbinsX()+1, 0, hist.GetNbinsY()+1)
      if w!=0:
        hist.Scale(1./w)
      else:
        print("can't normalize, it's an empty histogram")

   return hist


def MakeLatex(xPoint, yPoint, latexName):
    tex = TLatex(xPoint, yPoint,latexName)
    tex.SetNDC()
    tex.SetTextAlign(31)
    tex.SetTextFont(42)
    tex.SetTextSize(0.03)
    tex.SetLineWidth(2)
    return tex


def MakeLine(xlow, ylow, xup, yup):
  line = TLine(xlow,ylow,xup,yup)
  line.SetLineStyle(1)
  line.SetLineWidth(2)
  return line

def LegendMaker(energyPlots=False):
  legend1 = TLegend(0.6,0.7,0.85,0.85); ### without the mean
  legend1.SetFillColor(kWhite);
  legend1.SetTextFont(42);
  if(energyPlots):
      legend1.SetTextSize(0.048);
  else:
      legend1.SetTextSize(0.032);
  legend1.SetBorderSize(0);
  legend1.SetShadowColor(kWhite);
  legend1.SetFillStyle(0);
  return legend1;


def TexMaker(TexList, Atlas=False, Lumi=False, noRatio=False, do80Inv=False, do59Inv=False):
  if(Atlas):    
    TexList[0] = TLatex(0.4,0.80,"#sqrt{s}=13.6 TeV")
    TexList[0].SetNDC()
    TexList[0].SetTextAlign(31)
    TexList[0].SetTextFont(42)
    TexList[0].SetTextSize(0.037)
    TexList[0].SetLineWidth(2)
    
    TexList[1] = TLatex(0.22,0.874,"ATLAS") ### for the plots without mean
    TexList[1].SetNDC()
    TexList[1].SetTextAlign(13)
    TexList[1].SetTextFont(72) # helvetica italic
    TexList[1].SetTextSize(0.042)
    TexList[1].SetLineWidth(1)
    
    TexList[2] = TLatex(0.35,0.873,"Internal")
    TexList[2].SetNDC()
    TexList[2].SetTextAlign(13)
    TexList[2].SetTextFont(42)
    TexList[2].SetTextSize(0.037)
    TexList[2].SetLineWidth(1)


def SetHistColorEtc(hist, color):
     #hist = getOverflow(hist)
     hist.SetLineColor(color)
     #hist.GetYaxis().SetTitle("Events")
     hist.SetFillColor(0)
     if(color==kGreen+3):
       hist.SetMarkerStyle(kFullTriangleUp)
     elif(color==kGreen+2):
       hist.SetMarkerStyle(kFullTriangleUp)
     elif((color==2) or (color==kRed)):
       hist.SetMarkerStyle(kFullTriangleDown)
     elif ((color==4) or (color==kBlue)):
       hist.SetMarkerStyle(kFullSquare)
     elif (color==kBlack):
       hist.SetMarkerStyle(kOpenCircle)
     elif (color==kRed+3):
       hist.SetMarkerStyle(kFullDiamond)
     elif (color==kMagenta):
       hist.SetMarkerStyle(kFullCross)
     elif (color==kBlue+3):
       hist.SetMarkerStyle(kFullCircle)
     elif (color==kOrange+7):
       hist.SetMarkerStyle(kPlus)
     elif (color==kYellow):
       hist.SetMarkerStyle(kOpenStar)
     elif (color==kYellow+3):
       hist.SetMarkerStyle(kFullStar)
     elif (color==kViolet-2):
       hist.SetMarkerStyle(kFullStar)
     elif (color==kYellow-7):
       hist.SetMarkerStyle(kFullStar)
     elif (color==kBlue-9):
       hist.SetMarkerStyle(kMultiply)
     elif (color==kAzure+10):
       hist.SetMarkerStyle(kPlus)
     elif (color==kCyan-3):
       hist.SetMarkerStyle(kOpenSquare)
     elif (color==kViolet-6):
       hist.SetMarkerStyle(kOpenDiamond)
     elif (color==kGray):
       hist.SetMarkerStyle(kOpenSquare)
     elif (color==kAzure+1):
       hist.SetMarkerStyle(kMultiply)
     elif (color==kOrange+2):
       hist.SetMarkerStyle(kPlus)
     elif (color==kViolet-1):
       hist.SetMarkerStyle(kOpenDiamond)
     elif (color==kGreen-3):
       hist.SetMarkerStyle(kFullTriangleUp)
     else:
       hist.SetMarkerStyle(kDot)
       
     hist.SetMarkerColor(color)
     if (color==kBlack):
        hist.SetMarkerSize(0.5)
     else:
        hist.SetMarkerSize(0.7)
     return hist


def OneRatioAxisSize(h2, color=kBlack):
  h2.GetYaxis().SetRangeUser(0.0,4.5)
  h2.GetYaxis().SetLabelSize(0.12)
  h2.GetYaxis().SetNdivisions(4)
  h2.GetYaxis().CenterTitle()
  h2.GetYaxis().SetTitleOffset(0.45)
  h2.GetYaxis().SetTitleSize(0.1)
  
  h2.GetXaxis().SetLabelSize(0.12)
  h2.GetXaxis().SetTitleSize(0.14)
  h2.GetXaxis().SetTitleOffset(0.847)
  
  h2 = SetHistColorEtc(h2, color)
  
  h2.SetTitle('')
  return deepcopy(h2)


def SaveFile(c, CanvasName):
   #c.SaveAs(CanvasName+".C")
  #  c.SaveAs(CanvasName+".png")
   c.SaveAs(CanvasName+".pdf")
  #  c.SaveAs(CanvasName+".root")


def AxisLabelEtc(hist, yAxisName, xAxisName):
     hist.GetYaxis().SetTitle(yAxisName)
     hist.GetXaxis().SetTitle(xAxisName)
     hist.GetYaxis().SetTitleOffset(1.5) # 0.97
     hist.GetYaxis().SetLabelOffset(0.001)
     hist.GetYaxis().SetLabelSize(0.04)
     hist.GetYaxis().SetTitleSize(0.0434)
     hist.GetXaxis().SetTitleSize(0.037)
     hist.GetXaxis().SetLabelSize(0.032)
     hist.GetXaxis().SetLabelOffset(0.01)
     hist.GetXaxis().SetTitleOffset(1.05)
     hist.SetLineWidth(1)
     hist.SetTitle("")
     return hist



### prepare more than one histogram on one canvas
def DrawHists(FirstTH1, LegendName, PlotColor,xAxisName, yAxisName, xrange1down, xrange1up, yrange1down, yrange1up, CanvasName, yline1low, yline1up, drawline=False, logy=False, latexName='', latexName2 = '', latexName3='', leftLegend=False, doAtlas=False, doLumi=False, noRatio=False, do80=False, do59=False, drawPattern="", logz=False, logx=False, latexName4='', zAxisName='Entries per primary electron',zrange1down=0,zrange1up=0.07):
   debug = False
   if(debug): print("just entering plot code")
   ### without mean
   Tex  = MakeLatex(0.80,0.70,latexName)
   Tex2 = MakeLatex(0.80,0.64,latexName2)
   Tex3 = MakeLatex(0.80,0.58,latexName3)
   Tex4 = MakeLatex(0.34,0.52,latexName4)
   
   if(debug): print ("defining Tex ")
   c = TCanvas("c","c",500, 500)
   gStyle.SetOptStat(0)
   gPad.SetLeftMargin(0.15)
   gPad.SetRightMargin(0.14)
   gPad.SetBottomMargin(0.14)
   c.cd()
   c.SetGrid()
   
   if(logx):
     c.SetLogx()
   if(logy):
     c.SetLogy()
   if(logz):
     c.SetLogz()
     
   if(debug): print ("Set Logy ")
   line = MakeLine(xrange1down,yline1low,xrange1up,yline1up)

   if(debug): print ("Set Legend ")
   tex1 = TLatex(); tex2 = TLatex(); tex3 = TLatex()
   L = [tex1, tex2, tex3]
   TexMaker(L, doAtlas, doLumi, noRatio, do80, do59)
   stList = []
   if(debug): print ("Set Ranges ")
   integralList = []; integralListError = []
   strType = str(type(FirstTH1[0]))

   for i in range(0, len(FirstTH1)):     
     FirstTH1[i] = AxisLabelEtc(FirstTH1[i], yAxisName, xAxisName)
     if("TH2" not in strType):
        FirstTH1[i] = SetHistColorEtc(FirstTH1[i], PlotColor[i])
     else:
        FirstTH1[i] = SetHistColorEtc(FirstTH1[i], PlotColor[i])
        FirstTH1[i].SetLineColor(kWhite)
       
     FirstTH1[i].SetFillColor(0)
     if("TH2D" in strType):
        xBinMax = FirstTH1[i].GetNbinsX()
        yBinMax = FirstTH1[i].GetNbinsY()
        integralList.append(FirstTH1[i].Integral(0, xBinMax+1, 0, yBinMax+1))
     elif("TH1" in strType):
         w = FirstTH1[i].Integral(0, FirstTH1[i].GetNbinsX()+1)
         integralList.append(w)
     else:
        pass

     FirstTH1[i].GetYaxis().SetRangeUser(yrange1down,yrange1up)
     FirstTH1[i].GetXaxis().SetRangeUser(xrange1down,xrange1up)
     if zAxisName:
        FirstTH1[i].GetZaxis().SetTitle("Events")
     else:
        FirstTH1[i].GetZaxis().SetTitle(zAxisName)
     FirstTH1[i].SetMaximum(yrange1up)
     FirstTH1[i].SetMinimum(yrange1down)
   
   if(debug): print ("After for loop ")
   FirstTH1[0].GetXaxis().SetRangeUser(xrange1down,xrange1up)
   FirstTH1[0].GetXaxis().SetNdivisions(5)
   FirstTH1[0].GetZaxis().SetRangeUser(zrange1down,zrange1up)
       
   FirstTH1[0].GetXaxis().SetNdivisions(9)
   gPad.SetTickx()
   gPad.SetTicky()
      
   if "TH2" in strType:
       drawStyle = "COLZ"
   elif "TH1" in strType:
       drawStyle = "hist"
   else:
       drawStyle = "AL"
   
   FirstTH1[0].Draw(drawStyle)
   gPad.Update();
   gPad.RedrawAxis();
   if "TH2" in strType:
      FirstTH1[0].GetZaxis().SetLabelSize(0.02)
      FirstTH1[0].GetZaxis().SetTitleOffset(1.4)
      FirstTH1[0].GetYaxis().SetTitleOffset(1.1)
      FirstTH1[0].GetYaxis().SetLabelSize(0.03)
      try:
        pl = FirstTH1[0].GetListOfFunctions().FindObject("palette")
        pl.SetX1NDC(0.86);
        pl.SetX2NDC(0.88);
        pl.SetY1NDC(0.14);
        pl.SetY2NDC(0.90);
        gPad.Modified();
        gPad.Update();
      except:
        print("This histogram is empty: ", FirstTH1[0].GetName())
   
   if(debug): 
       print ("After first Draw ")
       print("len(FirstTH1): ", len(FirstTH1), " FirstTH1: ", FirstTH1)

   if(len(FirstTH1)>1):
    #### special file for Allpix-squared plot, otherwise remove -1 in the range
    for i in range(1, len(FirstTH1)):
      if(debug): print("drawing i=", i, " FirstTH1: ", FirstTH1[i])
      FirstTH1[i].Draw(drawStyle+" sames") 
      FirstTH1[i].SetFillColor(0)
        
   if(debug): print ("After Draw loop ")
   Tex.Draw("sames")
   Tex2.Draw("sames")
   Tex3.Draw("sames")
   Tex4.Draw("sames")
   L[0].Draw()
   L[1].Draw()
   L[2].Draw()
   
   SaveFile(c, CanvasName)
   return deepcopy(c)


### prepare more than one histogram on one canvas, then also plots their ratio
def DrawHistsRatio(FirstTH1, LegendName, PlotColor, xrange1down, xrange1up, yrange1down, yrange1up, xaxisTitle, CanvasName, h2, yline1low, yline1up, drawline=False, logy=False, LatexName='', LatexName2='', TeVTag=False, doSumw2=False, doAtlas=False, doLumi=False, noRatio=False, do80=False, do59=False,drawOption=""):
   Tex = MakeLatex(0.85,0.65,LatexName)
   Tex2 = MakeLatex(0.85,0.60,LatexName2)
   c = TCanvas("c","c",900, 900)
   c.SetGrid()
   pad1 = TPad('pad1','pad1',0,0.25,1,1)
   pad1.SetBottomMargin(0.0)
   pad1.SetFillStyle(0)
   pad1.SetGrid()
   pad1.Draw()
   pad1.cd()
   gStyle.SetOptStat(0)
   if(logy):
     pad1.SetLogy()
     
   if "energy" in FirstTH1[0].GetName() or "time" in FirstTH1[0].GetName():
    pad1.SetLogx()

   line = MakeLine(xrange1down,yline1low,xrange1up,yline1up)
   legend1 = LegendMaker()
   tex1 = TLatex(); tex2 = TLatex(); tex3 = TLatex()
   L = [tex1, tex2, tex3]
   TexMaker(L, doAtlas, doLumi, noRatio, do80, do59)
   FirstTH1[0].GetYaxis().SetRangeUser(yrange1down,yrange1up)
   FirstTH1[0].GetXaxis().SetRangeUser(xrange1down,xrange1up)
   for i in range(0, len(FirstTH1)):
     FirstTH1[i].SetTitle("")
     FirstTH1[i].GetYaxis().SetTitle(FirstTH1[i].GetYaxis().GetTitle())
     
     FirstTH1[i].GetYaxis().SetTitleOffset(0.95)
     FirstTH1[i].GetYaxis().SetTitleSize(0.05)
     FirstTH1[i].GetYaxis().SetLabelSize(0.045)
     FirstTH1[i].GetXaxis().SetTitle("")
     FirstTH1[i].GetXaxis().SetLabelSize(0.15)
     w = FirstTH1[i].Integral()
     FirstTH1[i] = SetHistColorEtc(FirstTH1[i], PlotColor[i])
     legend1.AddEntry(FirstTH1[i],LegendName[i]+" ("+str(round(w,2))+")", "l")
   
   
   FirstTH1[0].GetXaxis().SetLabelSize(0.0);
   FirstTH1[0].GetYaxis().SetTitle("Events/bin");
   FirstTH1[0].SetLineWidth(2)
   FirstTH1[0].Draw("hist")
    
   gPad.SetTickx()
   gPad.SetTicky()
   gPad.Modified(); 
   gPad.Update();
   
   #### special file for Allpix-squared plot, otherwise remove -1 in the range
   for i in range(1, len(FirstTH1)):
     FirstTH1[i].SetLineWidth(1)
     FirstTH1[i].Draw("hist sames")
     gPad.Modified(); 
     gPad.Update();
     
   FirstTH1[0].SetLineWidth(1)
   FirstTH1[0].Draw("hist sames")
    
   gPad.RedrawAxis()
   L[1].Draw("sames")
   L[2].Draw("sames")
   if(TeVTag):
       TexTeV = TLatex(0.892,0.914,"#sqrt{s}=13 TeV")
       TexTeV.SetTextAlign(31)
       TexTeV.SetTextFont(42)
       TexTeV.SetTextSize(0.037)
       TexTeV.SetLineWidth(2) 
       TexTeV.Draw()
   else:
       L[0].Draw()
   legend1.Draw("sames")
   Tex.Draw("sames")
   Tex2.Draw("sames")
   
   c.cd()
   pad2 = TPad("pad2", "pad2",0.0,0.0,1.0,0.245)
   pad2.SetTopMargin(0.0)
   pad2.SetBottomMargin(0.3)
   pad2.SetFillStyle(0)
   pad2.Draw()
   pad2.cd()
   gStyle.SetOptStat(0)
   gPad.SetTickx()
   gPad.SetTicky()
   
   if(doSumw2):
       for i in range(0, len(FirstTH1)):
          FirstTH1[i].Sumw2() 
   
   h2.Divide(FirstTH1[0],FirstTH1[1])
   h2.GetXaxis().SetRangeUser(xrange1down,xrange1up)
   h2.GetXaxis().SetTitle(xaxisTitle)
   h2.GetYaxis().SetTitle(h2.GetYaxis().GetTitle())
   h2.GetYaxis().CenterTitle()
   h2.SetTitle('')
   h2 = OneRatioAxisSize(h2)
   h2.GetYaxis().SetRangeUser(0.1, 2.5)
   h2.GetYaxis().SetTitleOffset(0.45)
   h2.Draw("ep")
   if(drawline):
     line.SetLineStyle(2)
     line.Draw()
   SaveFile(c, CanvasName)
   return deepcopy(c)

### tha main function which calls all the above functions
def main():
    gROOT.SetBatch()
    parser = argparse.ArgumentParser(description='Code to plot muon distributions')
    parser.add_argument('-i1', default="muonAnalysis_data23.root", type=str) ### use the Data histogram here
    parser.add_argument('-comp', action='store_true') ### turn this on when one wants to compare Data vs MC
    parser.add_argument('-inGrid', action='store_true') ### turn this on when running on grid
    parser.add_argument('-i2', default="user.asantra.zmumu.ANALYSIS_Histogram_All.root", type=str) ### use the MC histogram here
    args = parser.parse_args()

    inName1 = args.i1
    if(args.comp): inName2 = args.i2
    outDirExtension = inName1.split('.root')[0]
    
    outDir = "muCompDist_"+outDirExtension
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    ### get the root file
    if (args.inGrid):
      ### input directory where the histogram files live
      inDir1 = "/storage/agrp/arkas/MCPDataWorkArea/"
      inDir2 = "/storage/agrp/arkas/MCPWorkArea/"
    else:
      ### input directory where the histogram files live
      inDir1 = "/Users/arkasantra/arka/MCPWork/MCPFiles/HistFiles"
      inDir2 = inDir2
    
    ### reading the histogram files
    inFile1 = TFile(inDir1+"/"+inName1, "READ")
    if(args.comp): inFile2 = TFile(inDir2+"/"+inName2, "READ")

    ### add the histogram names that you want to plot
    histogramNames = ["mll", "nMuon", "muon_nprecisionLayers_0", "muon_nprecisionLayers_1", "muon_qOverPsignif_0", "muon_qOverPsignif_1", "muon_meqOverPsigma_0", "muon_meqOverPsigma_1", "muon_idqOverPsigma_0", "muon_idqOverPsigma_1", "muon_meqOverP_0", "muon_meqOverP_1", "muon_idqOverP_0", "muon_idqOverP_1","muon_pt_0", "muon_pt_1", "muon_pt_0", "muon_phi_0", "muon_phi_1", "muon_eta_0", "muon_eta_1", "muon_eta_vs_phi_0", "muon_eta_vs_phi_1"]



    #### plotting the signal and background
    TeVTag=False; doSumw2=False; doAtlas=True; doLumi=False; noRatio=False; do80=False; do59=False; drawOption=""
    drawline    = True
    leftLegend  = True

    ### looping over barrel and endcap region
    for sector in ["barrel" , "endcap"]:
    # for sector in ["barrel"]:
      print('---> plotting ', sector)
      #### plot the barrel distributions
      for names in histogramNames:
        if sector == "barrel":
           suf = ''
        else:
           suf = '_EC'
        ### get histograms
        firstTH1  = inFile1.Get(names+suf+"_medium")
        secondTH1 = inFile1.Get(names+suf+"_highpt")
        strType   = str(type(firstTH1))

        LegendName  = ["Medium", "High-p_{T}"]
        PlotColor   = [2, 4]
        latexName   = "muon"
        latexName2  = ''
        ### plot 2D COLZ plots
        if 'TH2' in strType:
          logy        = False
          logz        = True
          logx        = False
          drawPattern = "COLZ"

          ### get normalized result
          # firstTH1    = normalizeHist(firstTH1)

          FirstTH1    = [firstTH1]
          xAxisLow    = FirstTH1[0].GetXaxis().GetBinCenter(1)
          xAxisHigh   = FirstTH1[0].GetXaxis().GetBinCenter(FirstTH1[0].GetNbinsX())
          yAxisLow    = FirstTH1[0].GetYaxis().GetBinCenter(1)
          yAxisHigh   = FirstTH1[0].GetYaxis().GetBinCenter(FirstTH1[0].GetNbinsY())
          
          xAxisName   = FirstTH1[0].GetXaxis().GetTitle()
          yAxisName   = FirstTH1[0].GetYaxis().GetTitle()
          latexName3  = sector
          latexName4  = ''
          zAxisName   = 'Entries'
          zrange1down = 1e5
          zrange1up   = 1e7

          ## medium
          latexName2  = "medium"

          try:
            DrawHists(FirstTH1, LegendName, PlotColor,xAxisName, yAxisName, xAxisLow, xAxisHigh, yAxisHigh, yAxisLow, outDir+"/"+FirstTH1[0].GetName(), 1.0, 1.0, drawline, logy, latexName, latexName2, latexName3, leftLegend, doAtlas, doLumi, noRatio, do80, do59, drawPattern, logz, logx, latexName4, zAxisName,zrange1down,zrange1up)
          except:
             print("Can't plot ", FirstTH1[0].GetName())

          ## high pt
          latexName2   = "high-p_{T}"
          ### get normalized result
          # secondTH1    = normalizeHist(secondTH1)
          SecondTH1    = [secondTH1]
          
          try:
            DrawHists(SecondTH1, LegendName, PlotColor,xAxisName, yAxisName, xAxisLow, xAxisHigh, yAxisHigh, yAxisLow, outDir+"/"+SecondTH1[0].GetName(), 1.0, 1.0, drawline, logy, latexName, latexName2, latexName3, leftLegend, doAtlas, doLumi, noRatio, do80, do59, drawPattern, logz, logx, latexName4, zAxisName,zrange1down,zrange1up)
          except:
             print("Can't plot ", SecondTH1[0].GetName())

        ### plot 1D plots
        else:
          logy        = True
          latexName2  = sector
          FirstTH1    = [firstTH1, secondTH1]
          xAxisLow    = FirstTH1[0].GetXaxis().GetBinCenter(1)
          xAxisHigh   = FirstTH1[0].GetXaxis().GetBinCenter(FirstTH1[0].GetNbinsX())
          
          yAxisHigh   = FirstTH1[0].GetMaximum()*4e1
          yAxisLow    = 1e0
          xAxisTitle  = FirstTH1[0].GetXaxis().GetTitle()
          ### there is a problem in lead pt x axis title, this is a dirty fix
          if not xAxisTitle:
            print("The problem is here: ", FirstTH1[0].GetXaxis().GetTitle())
            print("The problem is here: ", FirstTH1[0].GetName())
            xAxisTitle = "p_{T} (lead) [GeV]"
          print("xAxisTitle = ", xAxisTitle)

          h2 = FirstTH1[0].Clone("h2")
          h2.Reset()
          h2.GetYaxis().SetTitle("#frac{medium}{highpt}")

          try:
            DrawHistsRatio(FirstTH1, LegendName, PlotColor, xAxisLow, xAxisHigh, yAxisLow, yAxisHigh, xAxisTitle, outDir+"/"+FirstTH1[0].GetName(), h2, 1.0, 1.0, drawline, logy, latexName, latexName2, TeVTag, doSumw2, doAtlas, doLumi, noRatio, do80, do59)
          except:
             print("Can't plot ", FirstTH1[0].GetName())



        ### if we want to compare between data and MC situation
        if(args.comp):
          LegendName  = ["Data", "MC"]
          firstTH1  = inFile1.Get(names+suf+"_medium")
          secondTH1 = inFile2.Get(names+suf+"_medium")
          xAxisTitle  = secondTH1.GetXaxis().GetTitle()
          print("Comprison plots: x axis title: ", xAxisTitle)
          strType = str(type(firstTH1))
          latexName   = "medium muon"
          ### plot 2D COLZ plots
          if 'TH2' in strType: 
             continue
          
          logy        = True
          latexName2  = sector
          ### get normalized result
          firstTH1    = normalizeHist(firstTH1)
          secondTH1   = normalizeHist(secondTH1)

          FirstTH1    = [firstTH1, secondTH1]
          xAxisLow    = FirstTH1[0].GetXaxis().GetBinCenter(1)
          xAxisHigh   = FirstTH1[0].GetXaxis().GetBinCenter(FirstTH1[0].GetNbinsX())
          yAxisHigh   = 4.0#FirstTH1[0].GetMaximum()*4e1
          yAxisLow    = 3e-4
          
          h2 = FirstTH1[0].Clone("h2")
          h2.Reset()
          h2.GetYaxis().SetTitle("#frac{Data}{MC}")

          try:
            DrawHistsRatio(FirstTH1, LegendName, PlotColor, xAxisLow, xAxisHigh, yAxisLow, yAxisHigh, xAxisTitle, outDir+"/"+FirstTH1[0].GetName()+"_MediumDataMC", h2, 1.0, 1.0, drawline, logy, latexName, latexName2, TeVTag, doSumw2, doAtlas, doLumi, noRatio, do80, do59)
          except:
             print("Can't plot ", FirstTH1[0].GetName())
          
          
          LegendName  = ["Data", "MC"]
          firstTH1  = inFile1.Get(names+suf+"_highpt")
          secondTH1 = inFile2.Get(names+suf+"_highpt")

          ### get normalized result
          firstTH1    = normalizeHist(firstTH1)
          secondTH1   = normalizeHist(secondTH1)

          latexName   = "high p_{T} muon"
          logy        = True
          latexName2  = sector
          FirstTH1    = [firstTH1, secondTH1]
          xAxisLow    = FirstTH1[0].GetXaxis().GetBinCenter(1)
          xAxisHigh   = FirstTH1[0].GetXaxis().GetBinCenter(FirstTH1[0].GetNbinsX())
          
          yAxisHigh   = 2.0 #FirstTH1[0].GetMaximum()*4e1
          yAxisLow    = 3e-4

          h2 = FirstTH1[0].Clone("h2")
          h2.Reset()
          h2.GetYaxis().SetTitle("#frac{Data}{MC}")
          try:
            DrawHistsRatio(FirstTH1, LegendName, PlotColor, xAxisLow, xAxisHigh, yAxisLow, yAxisHigh, xAxisTitle, outDir+"/"+FirstTH1[0].GetName()+"_HighPtDataMC", h2, 1.0, 1.0, drawline, logy, latexName, latexName2, TeVTag, doSumw2, doAtlas, doLumi, noRatio, do80, do59)
          except:
             print("Can't plot ", FirstTH1[0].GetName())
      
          ### plotting in pt bins
          if 'qOverP' in names:
          # if 'zzz' in names:
              LegendName  = ["Data", "MC"]
              # ptBins      = ['1'] 
              ptBins      = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10']
              # etaBins     = ['1'] 
              etaBins     = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10']
              muonOrder   = {'0':"leading", '1':"sub-leading"}
              
              for key in muonOrder:
                  for ptbinValue in ptBins:
                    firstTH1  = inFile1.Get(names+suf+"_highpt_ptBin"+ptbinValue+"_"+key)
                    secondTH1 = inFile2.Get(names+suf+"_highpt_ptBin"+ptbinValue+"_"+key)

                    ### get normalized result
                    firstTH1    = normalizeHist(firstTH1)
                    secondTH1   = normalizeHist(secondTH1)

                    latexName   = muonOrder[key]+" high p_{T} muon"
                    logy        = True
                    latexName2  = sector+", pt bin "+ptbinValue
                    FirstTH1    = [firstTH1, secondTH1]
                    xAxisLow    = FirstTH1[0].GetXaxis().GetBinCenter(1)
                    xAxisHigh   = FirstTH1[0].GetXaxis().GetBinCenter(FirstTH1[0].GetNbinsX())
                    
                    yAxisHigh   = 4.0 #FirstTH1[0].GetMaximum()*4e1
                    yAxisLow    = 3e-10

                    h2 = FirstTH1[0].Clone("h2")
                    h2.Reset()
                    h2.GetYaxis().SetTitle("#frac{Data}{MC}")

                    DrawHistsRatio(FirstTH1, LegendName, PlotColor, xAxisLow, xAxisHigh, yAxisLow, yAxisHigh, xAxisTitle, outDir+"/"+FirstTH1[0].GetName()+"_HighPtDataMC_ptBin"+ptbinValue+"_"+key, h2, 1.0, 1.0, drawline, logy, latexName, latexName2, TeVTag, doSumw2, doAtlas, doLumi, noRatio, do80, do59)
            
                  for etabinValue in etaBins:
                    firstTH1  = inFile1.Get(names+suf+"_highpt_etaBin"+etabinValue+"_"+key)
                    secondTH1 = inFile2.Get(names+suf+"_highpt_etaBin"+etabinValue+"_"+key)

                    print("-------> ##### plotting ", firstTH1.GetName())
                    ### get normalized result
                    firstTH1    = normalizeHist(firstTH1)
                    secondTH1   = normalizeHist(secondTH1)
                    print("-------> ##### plotting ", firstTH1.GetName())
                    latexName   = muonOrder[key]+" high p_{T} muon"
                    logy        = True
                    latexName2  = sector+", eta bin "+etabinValue
                    FirstTH1    = [firstTH1, secondTH1]
                    xAxisLow    = FirstTH1[0].GetXaxis().GetBinCenter(1)
                    xAxisHigh   = FirstTH1[0].GetXaxis().GetBinCenter(FirstTH1[0].GetNbinsX())
                    
                    yAxisHigh   = 4.0 #FirstTH1[0].GetMaximum()*4e1
                    yAxisLow    = 3e-10

                    h2 = FirstTH1[0].Clone("h2")
                    h2.Reset()
                    h2.GetYaxis().SetTitle("#frac{Data}{MC}")

                    DrawHistsRatio(FirstTH1, LegendName, PlotColor, xAxisLow, xAxisHigh, yAxisLow, yAxisHigh, xAxisTitle, outDir+"/"+FirstTH1[0].GetName()+"_HighPtDataMC_etaBin"+etabinValue+"_"+key, h2, 1.0, 1.0, drawline, logy, latexName, latexName2, TeVTag, doSumw2, doAtlas, doLumi, noRatio, do80, do59)
        
if __name__=="__main__":
   start = time.time()
   main()
   print("--- processing time: ", time.time() - start)
