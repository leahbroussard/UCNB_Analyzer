// Copyright 2016.  UT-Battelle, LLC.
// This file is part of UCNB_Analyzer.
// See LICENSE.md included in top directory of this distribution.

// Read calib files without using SiCalibrator: need to generate ROOT dictionaries for vector<struct>
// Bizarre spectra features for low shaping times: hard-code shaping time scan for specific peaks :(
// CebScan and related functions perform shaping scans
// PlotAlltheThings, PlotScanResults, PlotScan, FitENC2, SumResolutions, and Sumh3 plot results from UCNB_Analyzer program. -- still some development needed to clean up misbehaving spectra.

TFile *f = 0;
//3.62 at 300K, 3.72 at 80K
double eVtoENC = 3.7;


void SumPlot(int src);
void Recal(double cal0, double cal1);
void Open(int th, int d, int s, int t);
void Open(int th, int s, int t);
void ComparePlots(int ch);
void ShowPlots(int ch);
void CompareShaping(int src, int ch);
void CebScan(int ch);
void FitCeb(int ch, int shaping);
double ceb(double*x, double* par);
void CeXScan(int ch);
void FitCeX(int ch, int shaping, int top);
double cex(double*x, double* par);
double pe(double E);
void FitSnX(int ch, int shaping, int top);
double snx(double*x, double* par);
void LoopForFWHMX();
void PrintAlltheThings(int ch);
void PlotForPaper(int ch, const char* fitopt);
void PlotScanResults(int ch, const char* fitopt);
void FitENC2(TGraph* g);
void FitFWHM(TGraph* g);
void PlotScan(int ch);
void SumResolutions();
void Sumh3();
double invx(double* x, double* par);
double enc2(double* x, double* par);
double funch1(double* x, double* par);
double funch2(double* x, double* par);
double funch3(double* x, double* par);

void SumPlot(int src) {
	if (f == 0) return;
	TH2D* h2d;
	if (src == 1) h2d = (TH2D*)f->Get("hBi207x");
	if (src == 3) h2d = (TH2D*)f->Get("hSn113x");
	if (src == 5) h2d = (TH2D*)f->Get("hCe139x");
	TH1D* sum = (TH1D*)gROOT->FindObject("sum");
	if (sum != 0) delete sum;
	sum = new TH1D("sum","sum",15000,0,15000);
	for (int ch=0;ch<1;ch++) {
		TH1D* hsn = h2d->ProjectionY("hsn",ch+1,ch+1);
		TString fname = "fs";
		fname += src;
		fname += "ch";
		fname += ch;
		TF1* ff = (TF1*)gROOT->FindObject(fname);
		double calib = ff->GetParameter(2);
		for (int b=1;b<=hsn->GetNbinsX();b++) 
			sum->Fill(hsn->GetBinCenter(b),hsn->GetBinContent(b));
	}
	sum->Draw();
}

void Recal(double cal0, double cal1) {
	TH1D* sum = (TH1D*)gROOT->FindObject("sum");
	if (sum == 0) return;
	int nbins = sum->GetXaxis()->GetNbins(); 
	vector<double> new_bins(nbins+1); 
	double diff = 0;
	for(int i=0; i <= nbins; i++){ 
		new_bins[i] = cal0+sum->GetBinLowEdge(i+1)*cal1; 
		if (i > 0) diff = new_bins[i] - new_bins[i-1];
	}
	sum->SetBins(nbins, &new_bins[0]); 
	sum->Draw();
} 

void Open(int th, int d, int s, int t) {
	TString name = "Files/Sources/SumCalData_th";
	name += th;
	name += "d";
	name += d; name += "s"; name += s; name += "t"; name += t;
	name += ".root";
	if (f != 0) delete f;
	f = new TFile(name);
}

void Open(int th, int s, int t) {
	TString name = "Files/Sources/SumCalData_th";
	name += th;
	name += "d200";
	name += "s"; name += s; name += "t"; name += t;
	name += ".root";
	if (f != 0) delete f;
	f = new TFile(name);
}

void ComparePlots(int ch) {
	if (f==0) return;
	TString name;
	
	TH2D* hbi2 = (TH2D*)f->Get("hBi207x");
	TH1D* hbi = hbi2->ProjectionY("hbi",ch+1,ch+1);
	hbi->SetLineColor(kBlue);
	hbi->Draw();
	name = "fs1ch"; name+= ch;
	TF1* fb = (TF1*)f->Get(name);
	fb->SetLineColor(kBlue);
	fb->Draw("same");
	cout << fb->GetParameter(2) << endl;
	
	TH2D* hsn2 = (TH2D*)f->Get("hSn113x");
	TH1D* hsn = hsn2->ProjectionY("hsn",ch+1,ch+1);
	hsn->SetLineColor(kGreen);
	hsn->Draw("same");
	name = "fs3ch"; name+= ch;
	TF1* fs = (TF1*)f->Get(name);
	fs->SetLineColor(kGreen);
	fs->Draw("same");
	cout << fs->GetParameter(2) << endl;
	
	TH2D* hce2 = (TH2D*)f->Get("hCe139x");
	TH1D* hce = hce2->ProjectionY("hce",ch+1,ch+1);
	hce->SetLineColor(kRed);
	hce->Draw("same");
	name = "fs5ch"; name+= ch;
	TF1* fc = (TF1*)f->Get(name);
	fc->SetLineColor(kRed);
	fc->Draw("same");
	cout << fc->GetParameter(2) << endl;
}

void ShowPlots(int ch) {
	if (f==0) return;
	TString name;
	
	TH2D* hbi2 = (TH2D*)f->Get("hBi207x");
	TH1D* hbi = hbi2->ProjectionY("hbi",ch+1,ch+1);
	hbi->SetLineColor(kBlue);
	hbi->Draw();
	
	TH2D* hsn2 = (TH2D*)f->Get("hSn113x");
	TH1D* hsn = hsn2->ProjectionY("hsn",ch+1,ch+1);
	hsn->SetLineColor(kGreen);
	hsn->Draw("same");
	
	TH2D* hce2 = (TH2D*)f->Get("hCe139x");
	TH1D* hce = hce2->ProjectionY("hce",ch+1,ch+1);
	hce->SetLineColor(kRed);
	hce->Draw("same");
}

void CompareShaping(int src, int ch) {
	const int num = 11;
	int sscan[] = {50,60,70,100,150,200,250,300,350,400,450};
	Color_t cols[] = {kBlack,kGray+2,kGray,kRed+2,kRed,
		kOrange+7,kOrange+1,kOrange,kYellow+2,kGreen,
		kGreen+2,kGreen-1,kCyan,kCyan+3,kBlue,
		kBlue+3,kViolet+10,kViolet-5,kViolet};
		
	TString srcname[] = {"hBi207","hBi207x","hSn113","hSn113x","hCe139","hCe139x"};
	for (int i=num-1;i>=0;i--) {
		Open(25,200,sscan[i],300);
		//Open(200,sscan[i],sscan[i]);
		if (f!=0) {
			TH2D* h2d = (TH2D*)f->Get(srcname[src]);
			TString name = "hp"; name += i;
			TH1D* hp = h2d->ProjectionY(name,ch+1,ch+1);
			hp->SetDirectory(gROOT);
			hp->SetLineColor(cols[i]);
			if (i==num-1) hp->Draw();
			else hp->Draw("same");
		}
	}
}

/* Hard-coded nonsense to fit 139Ce beta */
void CebScan(int ch) {
	const int num = 9;
	//shape looks awful with shaping < 200
	double sscan[] = {100,150,200,250,300,350,400,450,500};
	double sigma[num];
	double ENC2[num];
	double time[num];
	for (int i=0;i<num;i++) {
		Open(200,sscan[i],300);
		//Open(200,sscan[i],sscan[i]);
		if (f!=0) {
			TH2D* h2d = (TH2D*)f->Get("hCe139x");
			TH1D* hp = h2d->ProjectionY("hp",ch+1,ch+1);
			hp->GetXaxis()->SetRangeUser(300,800);
			int maxbin = hp->GetMaximumBin();
			double calib = 129./(double)maxbin;
			hp->GetXaxis()->SetRangeUser(maxbin-150,maxbin+150);
			TF1* fitf = (TF1*) gROOT->FindObject("fitf");
			if (fitf != 0) delete fitf;
			fitf = new TF1("fitf",ceb,0,1000,5);
			fitf->SetParameters(calib,1,3,25,20);
			hp->Fit(fitf,"QN","",maxbin-150,maxbin+150);
			sigma[i] = fitf->GetParameter(2);
			cout << sscan[i] << ": sigma=" << sigma[i] << endl;
			ENC2[i] = sigma[i]*sigma[i]*1.e3*1.e3/eVtoENC/eVtoENC;
			time[i] = sscan[i]*4.e-3;
		}
	}
	TGraph* g = new TGraph(num,time,ENC2);
	g->SetMarkerStyle(20);
	g->Draw("AP");
}

void FitCeb(int ch, int shaping) {
	Open(200,shaping,shaping);
	if (f!=0) {
		TH2D* h2d = (TH2D*)f->Get("hCe139x");
		TH1D* hp = h2d->ProjectionY("hp",ch+1,ch+1);
		hp->SetDirectory(gROOT);
		hp->GetXaxis()->SetRangeUser(0,500);
		hp->Draw();
		hp->GetXaxis()->SetRangeUser(300,800);
		int maxbin = hp->GetMaximumBin();
		double calib = 129./(double)maxbin;
		hp->GetXaxis()->SetRangeUser(maxbin-150,maxbin+150);
		TF1* fitf = (TF1*) gROOT->FindObject("fitf");
		if (fitf != 0) delete fitf;
		fitf = new TF1("fitf",ceb,0,1000,5);
		fitf->SetParameters(calib,1,3,25,20);
		hp->Fit(fitf,"","",maxbin-150,maxbin+150);
	}
}

double ceb(double*x, double* par) {
	double xval = *x;
	double calib = par[0];
	double scale = par[1];
	double sigma = par[2];
	double exptail = par[3];
	double expamp = par[4];
	xval = xval*calib;
	const int num = 1;
	double E[] = {126.9329};
	double branch[] = {17.15};
	double loE = 100;
	double hiE = 137;
	if (xval < loE || xval > hiE) TF1::RejectPoint();
	double value = 0;
	for (int i=0;i<num;i++) {
		double amp = branch[i];
		double adc = E[i];
		value += amp*TMath::Exp(-1.*(xval-adc)*(xval-adc)/2./sigma/sigma);
	}
	value += expamp*scale*TMath::Exp(-1.*xval/exptail);
	return value*scale;
}

/* Hard-coded nonsense to fit 139Ce x-rays */
void CeXScan(int ch) {
	const int num = 14;
	double sscan[] = {35,50,60,70,100,150,200,250,300,350,400,450,500,600};
	//double sscan[] = {50,60,70,80,100,125,150,175,200,225,250,260,265,275,300,325,350,400,450,500};
	double sigma[num];
	double ENC2[num];
	for (int i=0;i<num;i++) {
		Open(200,sscan[i],sscan[i]);
		if (f!=0) {
			TH2D* h2d = (TH2D*)f->Get("hCe139x");
			TH1D* hp = h2d->ProjectionY("hp",ch+1,ch+1);
			hp->GetXaxis()->SetRangeUser(0,500);
			int maxbin = hp->GetMaximumBin();
			double calib = 33.5/(double)maxbin;
			TF1* fitf = (TF1*) gROOT->FindObject("fitf");
			if (fitf != 0) delete fitf;
			fitf = new TF1("fitf",cex,0,300,5);
			fitf->SetParameters(calib,1,3,25,20);
			hp->Fit(fitf,"QN","",100,300);
			sigma[i] = fitf->GetParameter(2);
			ENC2[i] = sigma[i]*sigma[i]*1.e3*1.e3/eVtoENC/eVtoENC;
			cout << sscan[i] << ": sigma=" << sigma[i] << endl;
		}
	}
	TGraph* g = new TGraph(num,sscan,ENC2);
	g->SetMarkerStyle(20);
	g->Draw("AP");
}

void FitCeX(int ch, int shaping, int top) {
	Open(200,shaping,top);
	if (f!=0) {
		TH2D* h2d = (TH2D*)f->Get("hCe139x");
		TH1D* hp = h2d->ProjectionY("hp",ch+1,ch+1);
		hp->SetDirectory(gROOT);
		hp->GetXaxis()->SetRangeUser(0,500);
		hp->Draw();
		int maxbin = hp->GetMaximumBin();
		double calib = 33.5/(double)maxbin;
		hp->GetXaxis()->SetRangeUser(maxbin-150,maxbin+150);
		TF1* fitf = (TF1*) gROOT->FindObject("fitf");
		if (fitf != 0) delete fitf;
		fitf = new TF1("fitf",cex,0,300,5);
		fitf->SetParameters(calib,1,3,25,2);
		double scale = hp->GetBinContent(maxbin)/fitf->Eval(maxbin);
		fitf->SetParameters(calib,scale,3,25,2);
		fitf->SetParLimits(0,calib*0.9,calib*1.1);
		//hp->Draw(); fitf->Draw("same");
		hp->Fit(fitf,"L","",100,300);
	}
}

double cex(double*x, double* par) {
	double xval = *x;
	double calib = par[0];
	double scale = par[1];
	double sigma = par[2];
	double exptail = par[3];
	double expamp = par[4];
	xval = xval*calib;
	const int num = 5;
	double E[] = {33.034,33.442,37.72,37.801,38.726};
	double branch[] = {22.5,41.0,3.95,7.62,2.46};
	//from NIST XCOM
	//double photoelec[] = {3.67,3.624,3.199,3.192,3.113}; //1e-7 cm^2/g
	double loE = 22.5;
	double hiE = 50;
	double value = 0;
	if (xval < loE || xval > hiE) {
		TF1::RejectPoint();
		return 0;
	}
	for (int i=0;i<num;i++) {
		double amp = branch[i]*pe(E[i]);
		double adc = E[i];
		value += amp*TMath::Exp(-1.*(xval-adc)*(xval-adc)/2./sigma/sigma);
	}
	value += expamp*scale*TMath::Exp(-1.*xval/exptail);
	return value*scale;
}


double pe(double E) {
	//www.nndc.bnl.gov
	double sidensity = 2.329; //g/cm^3
	double sithickness = 0.2; //cm
	//from fit to photoelectric cs from NIST XCOM
	return sidensity*sithickness*(1.313 - (120.095/E) + (3465.4/E/E));
}

void FitSnX(int ch, int shaping, int top) {
	Open(200,shaping,top);
	if (f!=0) {
		TH2D* h2d = (TH2D*)f->Get("hSn113x");
		TH1D* hp = h2d->ProjectionY("hp",ch+1,ch+1);
		hp->SetDirectory(gROOT);
		hp->GetXaxis()->SetRangeUser(10,500);
		hp->Draw();
		int maxbin = hp->GetMaximumBin();
		double calib = 24./(double)maxbin;
		cout << hp->GetMaximumBin() << endl;
		hp->GetXaxis()->SetRangeUser(maxbin-150,maxbin+150);
		TF1* fitf = (TF1*) gROOT->FindObject("fitf");
		if (fitf != 0) delete fitf;
		fitf = new TF1("fitf",snx,0,300,5);
		fitf->SetParameters(calib,1,3,10,100);
		cout << hp->GetBinContent(maxbin) <<"/"<<fitf->Eval(maxbin)<< endl;
		double scale = hp->GetBinContent(maxbin)/fitf->Eval(maxbin);
		fitf->SetParameters(calib,scale,3,10,100);
		fitf->SetParLimits(0,calib*0.9,calib*1.1);
		hp->Draw(); fitf->Draw("same");
		hp->Fit(fitf,"L","",100,300);
	}
}

double snx(double*x, double* par) {
	double xval = *x;
	double calib = par[0];
	double scale = par[1];
	double sigma = par[2];
	double exptail = par[3];
	double expamp = par[4];
	xval = xval*calib;
	const int num = 5;
	double E[] = {24.002,24.21,27.238,27.276,27.863};
	double branch[] = {28,51.8,4.66,9,2.39};
	//from NIST XCOM
	double photoelec[] = {5.124,5.078,4.487,4.48,4.382}; //1e-7 cm^2/g
	double loE = 15;
	double hiE = 50;
	double value = 0;
	if (xval < loE || xval > hiE) {
		TF1::RejectPoint();
		return 0;
	}
	for (int i=0;i<num;i++) {
		double amp = branch[i]*photoelec[i]/photoelec[0];
		double adc = E[i];
		value += amp*TMath::Exp(-1.*(xval-adc)*(xval-adc)/2./sigma/sigma);
	}
	value += expamp*scale*TMath::Exp(-1.*xval/exptail);
	return value*scale;
}

void LoopForFWHMX() {
	TH1D* hSn = new TH1D("hSn","hSn",1000,0,5);
	TH1D* hCe = new TH1D("hCe","hCe",1000,0,5);
	TH1D* hSnb = new TH1D("hCe","hSnb",1000,0,5);
	for (int ch=0;ch<48;ch++){
		if (ch != 5 && ch != 10 && ch != 11 && ch != 13 && ch != 14) {
			cout << "ch " << ch << endl;
			PlotScanResults(ch,"Q");
			TF1* foo = (TF1*)gROOT->FindObject("fitsnx");
			double Sminx = foo->GetMinimumX(10*4.e-3,1000*4.e-3);
			hSn->Fill(Sminx);
			cout << Sminx << endl;
			foo = (TF1*)gROOT->FindObject("fitcex");
			double Cminx = foo->GetMinimumX(10*4.e-3,1000*4.e-3);
			hCe->Fill(Cminx);
			cout << Cminx << endl;
			foo = (TF1*)gROOT->FindObject("fitsnb");
			double Sbminx = foo->GetMinimumX(10*4.e-3,1000*4.e-3);
			hSnb->Fill(Sbminx);
			cout << Sbminx << endl;
		}
	}
	hSn->Draw();
	hCe->SetLineColor(kRed);
	hCe->Draw("Same");
	hSnb->SetLineColor(kGreen);
	hSnb->Draw("same");
}


void PrintAlltheThings(int ch) {
	TCanvas* c = new TCanvas();
	double q = 1.60218e-10; //nC
	double TrapA3 = 1.67;
	double ustos = 1.e6;
	c->SetLogx();
	c->SetLogy();
		TLegend* leg = new TLegend(0.5,0.5,0.8,0.8);
		PlotScanResults(ch,"Q");
		TF1* foo = (TF1*)gROOT->FindObject("fitsnx");
		leg->AddEntry(foo,"Sn Xray (24 keV)","l");
		cout << "Ch" << ch << " Snx ";
		for (int p=0;p<3;p++)
			cout << foo->GetParameter(p) << "\t";
		double nA =  foo->GetParameter(2)*ustos*q/TrapA3;
		double minfwhm = 2.35*TMath::Sqrt(foo->GetMinimum(10*4.e-3,1000*4.e-3))*eVtoENC/1.e3;
		cout << endl<< nA <<  " nA; minfwhm=" << minfwhm << " at " << foo->GetMinimumX(10*4.e-3,1000*4.e-3) << endl;
		foo = (TF1*)gROOT->FindObject("fitcex");
		leg->AddEntry(foo,"Ce Xray (33 keV)","l");
		cout << "Ch" << ch << " Cex ";
		for (int p=0;p<3;p++)
			cout << foo->GetParameter(p) << "\t";
		nA =  foo->GetParameter(2)*ustos*q/TrapA3;
		minfwhm = 2.35*TMath::Sqrt(foo->GetMinimum(10*4.e-3,1000*4.e-3))*eVtoENC/1.e3;
		cout << endl << nA <<  " nA; minfwhm=" << minfwhm << " at " << foo->GetMinimumX(10*4.e-3,1000*4.e-3) << endl;
		foo = (TF1*)gROOT->FindObject("fitceb");
		leg->AddEntry(foo,"Ce beta (127 keV)","l");
		cout << "Ch" << ch << " Ceb ";
		for (int p=0;p<3;p++)
			cout << foo->GetParameter(p) << "\t";
		nA =  foo->GetParameter(2)*ustos*q/TrapA3;
		minfwhm = 2.35*TMath::Sqrt(foo->GetMinimum(10*4.e-3,1000*4.e-3))*eVtoENC/1.e3;
		cout << nA <<  " nA; minfwhm=" << minfwhm << endl;
		foo = (TF1*)gROOT->FindObject("fitsnb");
		leg->AddEntry(foo,"Sn beta (364 keV)","l");
		cout << "Ch" << ch << " Snb ";
		for (int p=0;p<3;p++)
			cout << foo->GetParameter(p) << "\t";
		nA =  foo->GetParameter(2)*ustos*q/TrapA3;
		minfwhm = 2.35*TMath::Sqrt(foo->GetMinimum(10*4.e-3,1000*4.e-3))*eVtoENC/1.e3;
		cout << nA <<  " nA; minfwhm=" << minfwhm << endl;
		foo = (TF1*)gROOT->FindObject("fitbib");
		leg->AddEntry(foo,"Bi beta (482 keV)","l");
		cout << "Ch" << ch << " Bib ";
		for (int p=0;p<3;p++)
			cout << foo->GetParameter(p) << "\t";
		nA =  foo->GetParameter(2)*ustos*q/TrapA3;
		minfwhm = 2.35*TMath::Sqrt(foo->GetMinimum(10*4.e-3,1000*4.e-3))*eVtoENC/1.e3;
		cout << nA <<  " nA; minfwhm=" << minfwhm << endl;
		cout <<"*****************************" << endl;
		leg->Draw("same");
		double Kb = 1.38e-23; // m^2 kg K /s^2
		double T = 180.; // K
		double R = 1.e9; // Ohm
		double IL = 2e-9; // A
		q = 1.602e-19; // C
		cout << "feedback resistor noise " << 2.*Kb*T/R << endl;
		cout << "leakage current noise " << q*IL << endl;
}

void PlotForPaper(int ch, const char* fitopt) {
	TCanvas* c= (TCanvas*)gROOT->FindObject("canv");
	if (c!=0) delete c;
	c = new TCanvas("canv","",600,400);
	c->SetLogx();
	c->SetLogy();
	gPad->SetLeftMargin(0.125);
	gPad->SetBottomMargin(0.175);
	gPad->SetTopMargin(0.05);
	gPad->SetRightMargin(0.05);
	TLegend* leg = new TLegend(0.4,0.7,0.94,0.94);
	leg->SetNColumns(2);
	leg->SetLineColor(kWhite);
	leg->SetFillColor(kWhite);
	TString gname = "g";
	gname += ch;
	/*
	TFile* f = new TFile("Files/Sources/ShapingScanBib.root");
	TGraph* g = (TGraph*)f->Get(gname);
	if (g!=0) {
	g->GetYaxis()->SetRangeUser(1.e5,1.e6);
	g->SetMarkerStyle(24);
	g->Draw("AP");
	TLegendEntry *entry=leg->AddEntry(g,"113Bi CE","LP");
	entry->SetTextFont(132);
	}
	TF1* fitf = new TF1("fitbib",enc2,0,10,3);
	fitf->SetParameters(20000,80000,80000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kBlack);
	fitf->SetParLimits(2,80000,150000); //doesnt want to fit right
	if (g!=0)
		g->Fit(fitf,fitopt); 
	fitf->Draw("same");
	TF1* ffh1 = new TF1("ffh1-bib",funch1,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(0));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	ffh1 = new TF1("ffh2-bib",funch2,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(1));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	ffh1 = new TF1("ffh3-bib",funch3,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(2));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	f->Close();
	TFile* f = new TFile("Files/Sources/ShapingScanSnb.root");
	TString gname = "g";
	gname += ch;
	TGraph* g = (TGraph*)f->Get(gname);
	if (g!=0) {
		g->GetYaxis()->SetRangeUser(1.e5,1.e6);
	g->Draw("P");
	entry=leg->AddEntry(g,"113Sn CE","LP");
	entry->SetTextFont(132);
	}
	TF1* fitf = new TF1("fitsnb",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kBlack);
	if (g!=0)
		g->Fit(fitf,fitopt); 
	fitf->Draw("same");
	TF1* ffh1 = new TF1("ffh1-snb",funch1,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(0));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	ffh1 = new TF1("ffh2-snb",funch2,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(1));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	ffh1 = new TF1("ffh3-snb",funch3,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(2));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	f->Close();
	f = new TFile("Files/Sources/ShapingScanCex.root");
	TGraph* g = (TGraph*)f->Get(gname);
	g->SetMarkerStyle(25);
	entry=leg->AddEntry(g,"139Ce XR","LP");
	entry->SetTextFont(132);
	g->Draw("P");
	fitf = new TF1("fitcex",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kBlack);
	if (g!=0)
		g->Fit(fitf,fitopt);
	fitf->Draw("same");
	TF1* ffh1 = new TF1("ffh1-cex",funch1,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(0));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	ffh1 = new TF1("ffh2-cex",funch2,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(1));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	ffh1 = new TF1("ffh3-cex",funch3,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(2));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
	f->Close();
	*/
	f = new TFile("Files/Sources/ShapingScanSnx.root");
	TGraph* g = (TGraph*)f->Get(gname);
	if (g!=0) {
		g->SetMarkerStyle(20);
		g->SetTitle("");
		g->Draw("AP");
	
		TAxis* ax = g->GetXaxis();
		ax->SetTitle("Shaping Time [#mus]");
		ax->CenterTitle();
		ax->SetLimits(0.1,3);
		ax->SetTitleOffset(1.25);
		ax->SetTitleSize(0.07);
		ax->SetLabelSize(0.07);
		ax->SetTitleFont(132);
		ax->SetLabelFont(132);
		ax = g->GetYaxis();
		ax->SetRangeUser(1e4,1e6);
		ax->SetTitle("ENC^{2} [e^{2}]");
		ax->CenterTitle();
		ax->SetTitleOffset(0.85);
		ax->SetTitleSize(0.07);
		ax->SetLabelSize(0.07);
		ax->SetTitleFont(132);
		ax->SetLabelFont(132);
		entry=leg->AddEntry(g,"Measured","P");
		entry->SetTextFont(132);
	}
	fitf = new TF1("fitsnx",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kBlack);
	if (g!=0)
		g->Fit(fitf,fitopt);
	fitf->Draw("same");
	entry=leg->AddEntry(fitf,"ENC^{2} fit","L");
	entry->SetTextFont(132);
	TF1* ffh1 = new TF1("ffh1-snx",funch1,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(0));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineWidth(1);
	ffh1->SetLineStyle(1);
	ffh1->Draw("same");
		entry=leg->AddEntry(ffh1,"h_{1}/#tau","L");
		entry->SetTextFont(132);
	ffh1 = new TF1("ffh2-snx",funch2,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(1));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineWidth(3);
	ffh1->SetLineStyle(2);
	ffh1->Draw("same");
		entry=leg->AddEntry(ffh1,"h_{2}","L");
		entry->SetTextFont(132);
	ffh1 = new TF1("ffh3-snx",funch3,0,10,1);
	ffh1->SetParameter(0,fitf->GetParameter(2));
	ffh1->SetLineColor(kBlack);
	ffh1->SetLineWidth(1);
	ffh1->SetLineStyle(4);
	ffh1->Draw("same");
		entry=leg->AddEntry(ffh1,"h_{3} #tau","L");
		entry->SetTextFont(132);
	f->Close();
	
	leg->Draw("same");
}

void PlotScanResults(int ch, const char* fitopt) {
	TFile* f = new TFile("Files/Sources/ShapingScanSnb.root");
	TString gname = "g";
	gname += ch;
	TGraph* g = (TGraph*)f->Get(gname);
	if (g!=0) {
	g->GetYaxis()->SetRangeUser(1.e5,1.e6);
	g->Draw("AP");
	}
	TF1* fitf = new TF1("fitsnb",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kBlack);
	if (g!=0)
	g->Fit(fitf,fitopt);
	f->Close();
	f = new TFile("Files/Sources/ShapingScanSnx.root");
	g = (TGraph*)f->Get(gname);
	if (g!=0) {
	g->SetMarkerColor(kBlue);
	g->Draw("P");
	double absmin = 1.e12;
	for (int i=0;i<g->GetN();i++) {
		double x,y;
		g->GetPoint(i,x,y);
		if (y < absmin) absmin = y;
	}
	double minfwhm = 2.35*TMath::Sqrt(absmin)*eVtoENC/1.e3;
	cout << "absolute min fwhm: " << minfwhm << endl;
	}
	fitf = new TF1("fitsnx",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kBlue);
	if (g!=0)
	g->Fit(fitf,fitopt);
	f->Close();
	f = new TFile("Files/Sources/ShapingScanCeb.root");
	g = (TGraph*)f->Get(gname);
	if (g!=0) {
	g->SetMarkerColor(kGreen);
	g->Draw("P");
	}
	fitf = new TF1("fitceb",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetParLimits(1,50000,150000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kGreen);
	if (g!=0)
	g->Fit(fitf,fitopt,"",0.55,10);
	f->Close();
	f = new TFile("Files/Sources/ShapingScanCex.root");
	g = (TGraph*)f->Get(gname);
	if (g!=0) {
	g->SetMarkerColor(kRed);
	g->Draw("P");
	}
	fitf = new TF1("fitcex",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kRed);
	if (g!=0)
	g->Fit(fitf,fitopt);
	f->Close();
	f = new TFile("Files/Sources/ShapingScanBib.root");
	g = (TGraph*)f->Get(gname);
	if (g!=0) {
	g->SetMarkerColor(kViolet);
	g->Draw("P");
	}
	fitf = new TF1("fitbib",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
	fitf->SetNpx(1000);
	fitf->SetLineColor(kViolet);
	if (g!=0)
	g->Fit(fitf,fitopt);
	f->Close();
}


void FitENC2(TGraph* g) {
	g->Draw("AP");
	/*
	int i=0;
	double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	do {g->GetPoint(i++,x1,y1);} while (i<g->GetN() && x1 < 50);
	do {g->GetPoint(i++,x2,y2);} while (i<g->GetN() && x2 < 100);
	double slope = (y1 - y2) / ((1/x1) - (1/x2));
	double offset = y1 - slope/x1;
	TF1* p1 = (TF1*) gROOT->FindObject("fp1");
	if (p1 != 0) delete p1;
	p1 = new TF1("fp1",invx,10,1000,2);
	p1->SetParameters(slope,offset);
	p1->Draw("same");return;
	g->Fit(p1,"","",0,100);return;
	double h1 = p1->GetParameter(0);
	TF1* p3 = (TF1*) gROOT->FindObject("fp3");
	if (p3 != 0) delete p3;
	p3 = new TF1("fp3","pol1",0,1000);
	g->Fit(p3,"","",350,1000);
	double h3 = p3->GetParameter(1);
	*/
	TF1* fitf = (TF1*) gROOT->FindObject("fitE");
	if (fitf != 0) delete fitf;
	fitf = new TF1("fitE",enc2,0,10,3);
	fitf->SetParameters(20000,80000,50000);
/*
	fitf->SetParLimits(0,10000,50000);
	fitf->SetParLimits(1,0,100000);
	fitf->SetParLimits(2,10000,100000);
	*/
	fitf->SetNpx(1000);
	/*
	fitf->FixParameter(0,h1);
	fitf->SetParameter(1,0);
	fitf->FixParameter(2,h3);
*/
	g->Fit(fitf);
}
void FitFWHM(TGraph* g) {
	int n = g->GetN();
	vector<double> x(n), y(n);
	for (int i=0;i<n;i++)
		g->GetPoint(i,x[i],y[i]);
	vector<double> yy(n);
	for (int i=0;i<n;i++) {
		yy[i] = TMath::Sqrt(y[i])*3.7*2.35/1.e3;
		g->SetPoint(i,x[i],yy[i]);
	}
	TGraph* g2 = new TGraph(n,&x[0],&yy[0]);
	g2->Draw("AP");;
	TF1* fitf = (TF1*) gROOT->FindObject("fitE");
	if (fitf != 0) delete fitf;
	fitf = new TF1("fitE",enc2,10,1000,3);
	g2->Fit(fitf);
}

void PlotScan(int ch) {
	//Use with ShapeScan.root
	TString gname = "g";
	gname += ch;
	TGraph* g = (TGraph*)gROOT->FindObject(gname);
	g->Draw("AP");
	TH1D* hh1 = (TH1D*)gROOT->FindObject("h1");
	TH1D* hh2 = (TH1D*)gROOT->FindObject("h2");
	TH1D* hh3 = (TH1D*)gROOT->FindObject("h3");
	double h1 = hh1->GetBinContent(ch+1);
	double h2 = hh2->GetBinContent(ch+1);
	double h3 = hh3->GetBinContent(ch+1);
	TF1* fitf = (TF1*) gROOT->FindObject("fitE");
	if (fitf != 0) delete fitf;
	fitf = new TF1("fitE",enc2,10*4.e-3,1000*4.e-3,3);
	fitf->SetParameters(h1,h2,h3);
	fitf->Draw("same");
}

void SumResolutions() {
	TH1D* h = (TH1D*)gROOT->FindObject("hminsig");
	TH1D* hfwhm = new TH1D("hfwhm","hfwhm",400,2,6);
	for (int i=1;i<48;i++) {
		double fwhm = 2.35 * h->GetBinContent(i);
		hfwhm->Fill(fwhm);
	}
	hfwhm->Draw();
	
}

void Sumh3() {
	TH1D* h = (TH1D*)gROOT->FindObject("h3");
	TH1D* hh = new TH1D("hsum3","hsum3",350,0.5e5,3.e5);
	for (int i=1;i<48;i++) {
		double sum = h->GetBinContent(i);
		hh->Fill(sum);
	}
	hh->Draw();
	//Average 1.5e6
	double Ileak = 2e-9/128.; // Amp per pixel = C/s
	double q = 1.6022e-19; // C/s
	double tau = 0.5e-6;
	double expect = TMath::Exp(1.)*TMath::Exp(1.)*tau*Ileak/4./q;
	cout << expect << endl;
	//http://www.ee.ktu.lt/journal/2007/5/08_T171_leak%20compensCMOS%20EE%2007%20naujas2%20_final2_.pdf
	//parallel (current) noise
	//ENC^2 = e^2 T_shape Ileak / 4 q (leakage contribution)
	//ENC^2 = e^2 T_shape k T / 2 q^2 Rp (thermal contribution)
	double kB = 1.38e-23; // J/K   J=C*V
	double Temp = 150; // K
	double R = 1.e9; //feedback resistor = GOhm
	double expect2 = TMath::Exp(1.)*TMath::Exp(1.)*tau*kB*Temp/2./q/q/R;
	cout << expect2 << endl;
	//http://www-physics.lbl.gov/~spieler/SLAC_Lectures/PDF/Sem-Det-II.pdf
	//p39 - different equation?
}

double invx(double* x, double* par) {
	double xval = *x;
	return par[0]/xval + par[1];
}

double enc2(double* x, double* par) {
	double xval = *x;
	return par[0]/xval + par[1] + par[2]*xval;
}

double funch1(double* x, double* par) {
	double xval = *x;
	return par[0]/xval;
}
double funch2(double* x, double* par) {
	double xval = *x;
	return par[0];
}
double funch3(double* x, double* par) {
	double xval = *x;
	return par[0]*xval;
}
