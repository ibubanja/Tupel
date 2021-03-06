void splithist(double ratio=0.2){
  TVirtualPad* pmain = TVirtualPad::Pad();
  
  double h = 1. - pmain->GetTopMargin() - pmain->GetBottomMargin();

  double xlow = 0.; //gStyle->GetPadLeftMargin();
  double xhigh = 1.; // - gStyle->GetPadRightMargin();

  double ytop = 1.; //- gStyle->GetPadTopMargin();
  double ybot = 0.; //gStyle->GetPadBottomMargin();
  double ymid = pmain->GetBottomMargin() + ratio * h; //ybot + ratio * (ytop-ybot);
  

  double yp1bot = ymid;
  double yp2top = ymid;

  TPad* p1 = new TPad(TString(pmain->GetName()) + "_1", pmain->GetTitle(),
		      xlow, yp1bot, xhigh, ytop);
  p1->SetNumber(1);
  TPad* p2 = new TPad(TString(pmain->GetName()) + "_2", pmain->GetTitle(), 
		      xlow, ybot, xhigh, yp2top);
  p2->SetNumber(2);
  p1->SetFillStyle(4000);
  p2->SetFillStyle(4000);

  double p1h = ytop - yp1bot;
  double p2h = yp2top - ybot;
  

/*
  p1->SetTopMargin(pmain->GetTopMargin()/p1h);
  p1->SetBottomMargin((ymid-yp1bot)/p1h);
  p1->SetLeftMargin(pmain->GetLeftMargin());
  p1->SetRightMargin(pmain->GetRightMargin());
  
  p2->SetTopMargin((ymid-yp2top)/p2h);
  p2->SetBottomMargin(pmain->GetBottomMargin()/p2h);
  p2->SetLeftMargin(pmain->GetLeftMargin());
  p2->SetRightMargin(pmain->GetRightMargin());
*/
    p1->SetTopMargin(0.11);
    p1->SetBottomMargin(0.);
    p1->SetRightMargin(0.02);
 
    p2->SetTopMargin(0.);
    p2->SetBottomMargin(0.3);
    p2->SetRightMargin(0.02);



  p2->Draw();
  p1->Draw();
  pmain->Modified();

  p1->cd();
}

void fixsplithist(TH1* htop, TH1* hbot){
  TVirtualPad* pmain = TVirtualPad::Pad()->GetCanvas();
  if(!pmain) return;
  TVirtualPad* p1 = pmain->cd(1);
  TVirtualPad* p2 = pmain->cd(2);

  if(!p1 || !p2) return;
  
  double scale = p1->GetHNDC() / p2->GetHNDC();

  double s = htop->GetYaxis()->GetLabelSize() * scale;
  double ss = htop->GetYaxis()->GetLabelSize();
  hbot->GetYaxis()->SetLabelSize(s);
  htop->GetYaxis()->SetLabelSize(ss);
  s = htop->GetYaxis()->GetTitleSize() * scale;
  hbot->GetYaxis()->SetTitleSize(s);

   hbot->GetYaxis()->SetTitleOffset(0.5);  
   htop->GetYaxis()->SetTitleOffset(0.5);  

  s = htop->GetYaxis()->GetLabelOffset() * scale;

  s = htop->GetXaxis()->GetLabelSize() * scale;
  hbot->GetXaxis()->SetLabelSize(s);
  htop->GetXaxis()->SetLabelSize(0.);
  s = htop->GetXaxis()->GetTitleSize() * scale;
  hbot->GetXaxis()->SetTitleSize(s);

 

  s = htop->GetXaxis()->GetLabelOffset() * scale;
  hbot->GetXaxis()->SetLabelOffset(s);  


  hbot->GetYaxis()->SetNdivisions(5);
}
void colorIt(TH1D *hMidA, int kCyan ){
	hMidA->SetMarkerColor(kCyan);
	hMidA->SetLineColor(kCyan);
	hMidA->SetMarkerStyle(20);//24
        hMidA->SetMarkerSize(0.7);
	hMidA->SetFillColor(kCyan);
	hMidA->SetFillStyle(0);	
        hMidA->GetXaxis()->SetLabelSize(0.05);
	hMidA->GetXaxis()->SetTitleSize(0.06);
        hMidA->GetYaxis()->SetTitleSize(0.05);
	hMidA->GetYaxis()->SetTitleOffset(0.9);
	hMidA->GetYaxis()->SetLabelSize(0.05);
	hMidA->SetTitleFont(42, "XYZ");
        hMidA->SetLabelFont(42, "XYZ");
        }
void colorBG(TH1D *hMidBG, int kBG ){
        hMidBG->SetLineWidth(0.0);
        hMidBG->SetMarkerColor(kBG);
        hMidBG->SetFillColor(kBG);
        hMidBG->SetLineColor(kBG);
        }

void      macro_tmp(std::string var1="__HISTNAME__",
		   char *Xtitle="__TITLE__",
		   int rebin = __NBIN__,
                   int isMu=__ISMU__
		  
 )
{
//	double lumi=19701;
	double lumi=19701;
    if(isMu==1)lumi=19545.;
    static int num_files=15;
    double scale_array[17];
    scale_array[0]=1.;
 //  scale_array[1]=3531.5*lumi/74212048.; //MC
     scale_array[1]=3531.5*lumi/30459425.; //MC
 //   Double_t Mc_scale= 3531.5*lumi/30350000.;
    scale_array[2]=245*lumi/6923735.;//tt
    //Double_t tt_scale= 234*lumi/6923735.;
    scale_array[3]=54.838*0.10608*lumi/1928399.;//WWlNu
    //Double_t wwjLNu_scale= 54.838*0.10608*lumi/1928399.;
    scale_array[4]=33.21*0.068258*lumi/3215983.;//WZLQ
    //Double_t wzjLQ_scale= 33.21*0.068258*lumi/3215983.;
    scale_array[5]=33.21*0.032887*lumi/2017975.;//WZLNu
    //Double_t wzjLNu_scale=33.21*0.032887*lumi/2017975.;
 
  scale_array[6]=17.654*0.04039*lumi/964911.;//ZZLNU
   // Double_t zzjLNu_scale= 8.258*0.04039*lumi/964911.;
    scale_array[7]=17.654*0.010196*lumi/4807886.;//ZZL
   // Double_t zzjL_scale= 8.258*0.14118*lumi/4807886.;
    scale_array[8]=17.654*0.14118*lumi/1936727.;//ZZLQ
   // Double_t zzjLQ_scale= 8.258*0.010196*lumi/1936727.;

 /*   scale_array[6]=8.258*0.04039*lumi/964911.;//ZZLNU
   // Double_t zzjLNu_scale= 8.258*0.04039*lumi/964911.;
    scale_array[7]=8.258*0.010196*lumi/4807886.;//ZZL
   // Double_t zzjL_scale= 8.258*0.14118*lumi/4807886.;
    scale_array[8]=8.258*0.14118*lumi/1936727.;//ZZLQ
   // Double_t zzjLQ_scale= 8.258*0.010196*lumi/1936727.;
*/
    scale_array[9]=3.79*lumi/259960.;
    //Double_t t_s_scale  = 3.79*lumi/259960.;
    scale_array[10]= 11.1*lumi/497657.;
    //Double_t t_tw_scale =11.1*lumi/497657.;
    scale_array[11]=56.4*lumi/3681614.;
    //Double_t t_t_scale =56.4*lumi/3681614.;
    scale_array[12]=1.76*lumi/139974.;
    //Double_t tbar_s_scale =1.76*lumi/139974.;
    scale_array[13]=11.1*lumi/482704.;
    //Double_t tbar_tw_scale =11.1*lumi/482704.;
    scale_array[14]= 30.7*lumi/1923607.;
    //Double_t tbar_t_scale =30.7*lumi/1923607.;
    scale_array[15]=36864.*lumi/16777216.;
    //Double_t wjets_scale =36864.*lumi/16777216.;

                   std::string filnam[]={"DoubleElec","DYJetstoLL","tt","wwj2L2Nu","wzj2L2Q","wzj3LNu","zzj2L2Nu","zzj4L","zzj2L2Q","T_s","T_tW","T_t","Tbar_s","Tbar_tW","Tbar_t","WJets"};
                   char nam[500];
  		   TFile *farray[100][100];
                   TH1D *histarray[100][100];
                   int ptthr=20;
                   int istight=0;
                   int dunf=0;
                   int ZPTweightcorr=0;
		    int JES=0;

               for(const int file_index=0;file_index<num_files;file_index++){
                 for(int ss_index=0;ss_index<13;ss_index++){    
      //sprintf(nam,"../MD_%s_doUnf_%i_isSS_%i_jetPt_%i_08_12.root",filnam[file_index].c_str(),0,ss_index,ptthr);
      //sprintf(nam,"../MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_ZPTweightcorr_%d_18_02.root",filnam[file_index].c_str(),isMu,dunf,ss_index,ptthr,istight,ZPTweightcorr);

//if(file_index==0)sprintf(nam,"../MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_ZPTweightcorr_%d_19_02.root",filnam[file_index].c_str(),isMu,1,ss_index,ptthr,0,0,ZPTweightcorr);
 
	if(ss_index==1){
	  sprintf(nam,"../rootfiles/fake_ss.root"); //same sign fake not used!!
	}
	  if(file_index==0){
	    if(ss_index==0 ){
		sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",filnam[file_index].c_str(),isMu,1,ss_index,ptthr,0,JES,ZPTweightcorr);
	    }
	    else if(ss_index==2 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,1,ZPTweightcorr);
	    }//JES up
	    else if(ss_index==3 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,-1,ZPTweightcorr);//JES down
	    }  

	    else if(ss_index==12 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_1_ZPTweightcorr_%d_CT10ind_-1_27_04.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,0,ZPTweightcorr);
	    }//JES up
	    else if(ss_index==13 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_-1_ZPTweightcorr_%d_CT10ind_-1_27_04.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,0,ZPTweightcorr);//JES down
	    }  
	  } 
	
	if(file_index==1){
	  if (ss_index==0){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",isMu,1,ss_index,ptthr,0,0,ZPTweightcorr);
//	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_1_doUnf_1_isSS_0_jetPt_20_Tightjet_0_JES_0_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_Sherpa_weight_cut_positive_22387200_27_04_1.root");
	  }
	  else if (ss_index==8){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_1_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//JER+
	  }
	  else if(ss_index==9){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_-1_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//JER-
	  }
	  else if(ss_index==10){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_1_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//PU+
	  }
	  else if (ss_index==11){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_-1_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//PU-
	  }
	  else if (ss_index==14){
	    sprintf(nam,"../rootfiles/Sherpa1.4_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//Sherpa1.4
	  }
/*	  else if (ss_index>11){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_%d_27_04.root",isMu,1,0,ptthr,0,0,ZPTweightcorr,ss_index-12);//PDF
	  }*/


        }
	if(file_index>1&&ss_index!=1){
	  sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_PU_subt_fwd_0_ZPTweightcorr_%d_CT10ind_-1_27_04.root",filnam[file_index].c_str(),isMu,1,0,20,0,0,ZPTweightcorr); 

        }    
      
                   //cout<<nam<<endl;
  		   farray[file_index][ss_index]  = TFile::Open(nam);
                   histarray[file_index][ss_index]  =(TH1D*)(farray[file_index][ss_index]->Get(var1.data()));
//	           if(var1.find("jet_pt_y_5")!= std::string::npos)histarray[file_index][ss_index]->GetXaxis()->SetLimits(0.,351.);
//		   if( var1.find("jet_pt_y_6")!= std::string::npos)histarray[file_index][ss_index]->GetXaxis()->SetLimits(0.,198.0);
                   histarray[file_index][ss_index]->Rebin(rebin);
                   if(ss_index!=12)histarray[file_index][ss_index]->Scale(scale_array[file_index]);
                   if(file_index ==1 && ss_index==12){
			 TTree *tree1 = (TTree *)farray[file_index][ss_index]->Get("tree");
                          double totw;
			  tree1->SetBranchAddress("cnt0",&totw);
			 tree1->GetEntry(0);


			histarray[file_index][ss_index]->Scale(3531.5*lumi/totw);
                   }
 		   double binn = histarray[file_index][ss_index]->GetNbinsX();

    		  for(int bb=1;bb<=binn;bb++){
     		    double acc = histarray[file_index][ss_index]->GetBinWidth(bb);
      		    double scale =  histarray[file_index][ss_index]->GetBinContent(bb)/acc;
      		    double scale_err =  histarray[file_index][ss_index]->GetBinError(bb)/acc;
   		    histarray[file_index][ss_index]->SetBinContent(bb,scale);
      		    histarray[file_index][ss_index]->SetBinError(bb,scale_err);
    		  }
                }
	      }

   THStack *hs = new THStack("hs","Stacked 1D histograms");
 //  THStack *hs2 = new THStack("hs2","Stacked 1D histograms SS");

   gStyle->SetOptStat(kFALSE);
   gStyle->SetOptTitle(kFALSE);
   gROOT->SetBatch(kTRUE);


    TH1D *bg_ss= (TH1D*)histarray[2][1]->Clone("bg_ss");
    for(int file_index=3;file_index<num_files;file_index++){
    bg_ss->Add(histarray[file_index][1]);
    }
//  mc_bg->Add(wjets);

    TH1D *mc_bg_ss= (TH1D*)histarray[1][1]->Clone("mc_bg_ss");
    mc_bg_ss->Add(bg_ss);

    TH1D *tot_ss= (TH1D*)histarray[0][1]->Clone("tot_ss");
    tot_ss->Add(mc_bg_ss,-1.0);

    TH1D *singleTop= (TH1D*)histarray[9][0]->Clone("singleTop");
    for(int file_index=10;file_index<15;file_index++){
      singleTop->Add(histarray[file_index][0]);
    }


  TH1D *mc_bg= (TH1D*)histarray[1][0]->Clone("mc_bg");
    for(int file_index=2;file_index<num_files;file_index++){
      mc_bg->Add(histarray[file_index][0]);
    }
 
  TH1D *mc_bg_sherpa= (TH1D*)histarray[1][12]->Clone("mc_bg");
    for(int file_index=2;file_index<num_files;file_index++){
      //mc_bg_sherpa->Add(histarray[file_index][0]);
    }

     for(int i=1;i<=histarray[0][0]->GetNbinsX();i++){
       cout<<i<<"  "<<  mc_bg->GetBinContent(i)-histarray[1][0]->GetBinContent(i)<<endl;
     } 
    TH1D *diboson= (TH1D*)histarray[3][0]->Clone("diboson");
    for(int file_index=4;file_index<=8;file_index++){
      diboson->Add(histarray[file_index][0]);
    }

  colorBG(tot_ss,kRed);
  colorBG(histarray[2][0], kPink - 6);
  colorBG(singleTop, kBlue);
   if(num_files>15)colorBG(histarray[15][0], kGreen);
  colorBG(diboson,kOrange - 3); 
  colorBG(histarray[1][0], kAzure + 4);
  colorBG(mc_bg, kAzure + 4);
  tot_ss->SetFillColor(kRed);
  histarray[2][0]->SetFillColor(kPink - 6);
  singleTop-> SetFillColor(kBlue);
   if(num_files>15)histarray[15][0]-> SetFillColor(kGreen);
  diboson->SetFillColor(kOrange - 3); 
  histarray[1][0]->SetFillColor(kAzure + 4);
  mc_bg->SetFillColor(kAzure + 4);


  hs->Add(histarray[2][0]);
  hs->Add(singleTop);
  if(num_files>15)hs->Add(histarray[15][0]);
 // hs->Add(tot_ss);
  hs->Add(diboson);
  hs->Add(histarray[1][0]);

//hs2->Add(mc_bg_ss);
//hs2->Add(data_ss);

  TH1D *mc_hs= (TH1D*)histarray[1][0]->Clone("mc_hs");
  hs->SetHistogram(mc_hs);


double MaxPlot_mc = (mc_bg->GetMaximum(10000000000000))*6.;
double MaxPlot_data = (histarray[0][0]->GetMaximum(10000000000000))*6.;
double MaxPlot =0;
if(MaxPlot_mc>MaxPlot_data) MaxPlot = MaxPlot_mc;
else MaxPlot = MaxPlot_data;

cout<<MaxPlot_mc<<"  "<<MaxPlot_data<<"  "<<MaxPlot<<endl;


  double MinPlot=histarray[2][0]->GetMinimum(0.000000000000000000000000000001);  

 //   double MinPlot_mc = mc_bg->GetMinimum(0.000000000000000000000000000001);
//    double MinPlot_data = histarray[0][0]->GetMinimum(0.000000000000000000000000000001);

//if(MinPlot_mc<MinPlot_data) MinPlot = MaxPlot_mc;
//else MinPlot = MinPlot_data;

//for(int i=0; i<num_files;i++){
//  double temp=histarray[i][0]->GetMinimum(0.000000000000000000000000000001);
//  if (temp<MinPlot)MinPlot=temp;/
//}

  //////////////////////////

  TH1D *Mont= (TH1D*)mc_bg->Clone("Mont");
  TH1D *Mont2= (TH1D*)mc_bg_sherpa->Clone("Mont");

//   double xLowLeg(0.71), xHighLeg(0.755);
//  leg1 = new TLegend(xLowLeg, 0.53, xHighLeg, 0.77);
  double xLowLeg(0.79), xHighLeg(0.89);
  leg1 = new TLegend(xLowLeg, 0.65, xHighLeg, 0.87);
  //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
  leg1->SetBorderSize(0);
  leg1->SetFillColor(kWhite);
  leg1->SetTextSize(0.03);
  leg1->AddEntry(histarray[2][0], "TTJets","peF");
  leg1->AddEntry(singleTop, "Single Top","peF");
  if(num_files>15)leg1->AddEntry(histarray[15][0], "WJets","peF");
  //leg1->AddEntry(tot_ss,"QCD","peF");
  leg1->AddEntry(diboson, "VVJets","peF");
  leg1->AddEntry(mc_bg, "DYJets MG","peF");
  //leg1->AddEntry(mc_bg_sherpa, "DYJets Sherpa","peF");
  if(isMu==0)leg1->AddEntry(histarray[0][0], "ee Data","pe");
  if(isMu==1)leg1->AddEntry(histarray[0][0], "#mu#mu Data","pe");
  leg1->SetTextFont(42);
 // leg1->AddEntry(wjets, "WJets","peF");


  
  leg2 = new TLegend(0.711567,0.653719,0.930511,0.965385);
  leg2->SetBorderSize(0);
  leg2->SetFillColor(kWhite);
  leg2->SetTextSize(0.05);
  leg2->AddEntry(Mont, "MC/Data","p");


  Mont->Divide(mc_bg,histarray[0][0],1.0,1.0);
  Mont2->Divide(mc_bg_sherpa,histarray[0][0],1.0,1.0);
  //Mont->Add(mc_bg,-1);
  double ntot_data=0.;
  double ntot_mc=0.;


  TCanvas *c1 = new TCanvas("c1","", 700, 900);

TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
pt->SetFillColor(0);
   pt->SetBorderSize(0);
   pt->SetLineColor(0);
   pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");

pt->Draw();

          c1->cd(); 
          splithist(0.2);
          c1->cd(1);
          gPad->SetLogy();  
	  colorIt(histarray[0][0],kBlack);
          hs->SetMaximum(MaxPlot*0.5);
          hs->SetMinimum(MinPlot/100);
	  hs->GetHistogram()->GetYaxis()->SetLabelSize(0.05);
	  hs->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
	  hs->GetHistogram()->GetYaxis()->SetTitleOffset(0.89);
	  hs->GetHistogram()->SetYTitle("#Events");
	  hs->Draw("hhist");
         
	  histarray[0][0]->Draw("eX0Csames");
	  mc_bg_sherpa->SetLineColor(kGreen);
	  mc_bg_sherpa->SetLineStyle(2);
	  mc_bg_sherpa->SetLineWidth(3);
          //mc_bg_sherpa->Draw("sames");
          leg1->Draw("same");


	  c1->cd(2);
          gPad->SetGridy();

	  colorIt(Mont,kBlack);
	  Mont->SetMinimum(0.5);
	  Mont->SetMaximum(1.5);
	  Mont->SetXTitle(Xtitle); 
	  Mont->GetXaxis()->SetTitleOffset(0.8);
          Mont->SetYTitle("MC/Data");
	  Mont->Draw("peX0C");// rec/gen
	  //Mont2->Draw("sames");// rec/gen
          fixsplithist(histarray[0][0],Mont);

          if(var1.find("jet_pt_y_fb_5")!= std::string::npos)hs->GetHistogram()->GetXaxis()->SetRangeUser(0.,348.0);
          if(var1.find("jet_pt_y_fb_6")!= std::string::npos)hs->GetHistogram()->GetXaxis()->SetRangeUser(0.,198.0);
//	  c1->Print("histos_note/__HISTNAME__.root");
         if(var1.find("jet_pt_y_fb_5")!= std::string::npos)Mont->GetXaxis()->SetRangeUser(0.,348.0);
          if(var1.find("jet_pt_y_fb_6")!= std::string::npos)Mont->GetXaxis()->SetRangeUser(0.,198.0);
          cout<<Mont->GetBinContent(1)<<endl;
          sprintf(nam,"histos_note_2801_dl/__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.pdf",isMu,dunf,ptthr,istight,ZPTweightcorr);
	  c1->Print(nam);
          sprintf(nam,"histos_note_2801_dl/__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.png",isMu,dunf,ptthr,istight,ZPTweightcorr);
	  c1->Print(nam);
          sprintf(nam,"histos_note_2801_dl/__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.C",isMu,dunf,ptthr,istight,ZPTweightcorr);
	  c1->Print(nam);


 TCanvas *c2 = new TCanvas("c2","", 700, 900);
// TPaveText *pt = new TPaveText(.05,.1,.95,.8);
//pt->SetLabel("CMS Preliminary #sqrt{s} = 8 TeV");
//          pt->Draw();
TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
pt->SetFillColor(0);
   pt->SetBorderSize(0);
   pt->SetLineColor(0);
   pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");

pt->Draw();

          c2->cd(); 
          splithist(0.5);
          c2->cd(1);
          gPad->SetLogy();  
          bg_ss->SetMaximum(MaxPlot*0.1);
          bg_ss->SetMinimum(0.01);
	  bg_ss->SetYTitle("#Events");
	  colorIt(histarray[0][1],kBlack);
          colorBG(bg_ss, kPink - 6);
          colorBG(mc_bg_ss,kAzure + 4);
          bg_ss->Draw("hhist");
          mc_bg_ss->Draw("hhistsames");

          histarray[0][1]->Draw("peX0Csames");


          c2->cd(2);
	  colorIt(tot_ss,kBlack);
          tot_ss->Draw("eX0C");
	  tot_ss->SetXTitle(Xtitle);
	  tot_ss->SetYTitle("SS Data - MC"); 

          sprintf(nam,"histos_note_1002/SS__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.pdf",isMu,dunf,ptthr,istight,ZPTweightcorr);
	  c2->Print(nam);
          sprintf(nam,"histos_note_1002/SS__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.png",isMu,dunf,ptthr,istight,ZPTweightcorr);
	  c2->Print(nam);
          sprintf(nam,"histos_note_1002/SS__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.C",isMu,dunf,ptthr,istight,ZPTweightcorr);
	  c2->Print(nam);


  ofstream myfile;
          sprintf(nam,"histos_note_1002/__HISTNAME___ismu_%d_doUnf_%d_PTJ_%i_tight_%i_ZPTweightcorr_%i.tex",isMu,dunf,ptthr,istight,ZPTweightcorr);
  myfile.open (nam);
 myfile<<"Njet & ";
          for(int i=1;i<=histarray[0][0]->GetNbinsX();i++){
	    myfile<<i-1<<"& ";
          }
          myfile<<"\\\\"<<endl;
          myfile<<"\\hline"<<endl;
          myfile<<"WJets & ";		
          for(int i=1;i<=histarray[15][0]->GetNbinsX();i++){
            myfile<<histarray[15][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
          myfile<<"ZZJets4L & ";		
          for(int i=1;i<=histarray[7][0]->GetNbinsX();i++){
            myfile<<histarray[7][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
          myfile<<"Single Top & ";		
          for(int i=1;i<=singleTop->GetNbinsX();i++){
            myfile<<singleTop->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
          myfile<<"ZZJets2L2Nu & ";		
          for(int i=1;i<=histarray[6][0]->GetNbinsX();i++){
            myfile<<histarray[6][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
          myfile<<"ZZJets2L2Q & ";		
          for(int i=1;i<=histarray[8][0]->GetNbinsX();i++){
            myfile<<histarray[8][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
          myfile<<"WWJets2L2Nu & ";		
          for(int i=1;i<=histarray[3][0]->GetNbinsX();i++){
            myfile<<histarray[3][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
         myfile<<"WZJets2L2Q & ";		
          for(int i=1;i<=histarray[4][0]->GetNbinsX();i++){
            myfile<<histarray[4][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
         myfile<<"WZJets3LNu & ";		
          for(int i=1;i<=histarray[5][0]->GetNbinsX();i++){
            myfile<<histarray[5][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
         myfile<<"TTJets & ";		
          for(int i=1;i<=histarray[2][0]->GetNbinsX();i++){
            myfile<<histarray[2][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
         myfile<<"DY+jets & ";		
          for(int i=1;i<=histarray[1][0]->GetNbinsX();i++){
            myfile<<histarray[1][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
  myfile<<"\\hline"<<endl;
        myfile<<"TOTAL & ";		
          for(int i=1;i<=mc_bg->GetNbinsX();i++){
            myfile<<mc_bg->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
myfile<<"\\hline"<<endl;
          myfile<<"DATA & ";		
          for(int i=1;i<=histarray[0][0]->GetNbinsX();i++){
            myfile<<histarray[0][0]->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
        myfile<<"Ratio & ";		
          for(int i=1;i<=Mont->GetNbinsX();i++){
            myfile<<Mont->GetBinContent(i)<<" & ";
          }
myfile<<"\\\\"<<endl;
  myfile<<"\\hline"<<endl;
  myfile<<"\\end{tabular}" <<endl;
  myfile<<"\\label{tab:njetunf}" <<endl;
     myfile<<"\\end{center}" <<endl;
  myfile<<"\\end{table}" <<endl;
  myfile.close();


}




