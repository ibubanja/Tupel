/*

Code by: Bugra Bilin, Kittikul Kovitanggoon, Tomislav Seva, Efe Yazgan, ...

*/
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <TLorentzVector.h>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "EgammaAnalysis/ElectronTools/interface/EGammaCutBasedEleId.h"
//#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "PhysicsTools/SelectorUtils/interface/CutApplicatorBase.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "FWCore/Framework/interface/GenericHandle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
//#include "CMGTools/External/interface/PileupJetIdentifier.h"
#include "EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/BTauReco/interface/JetTag.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
//#include "CondFormats/DataRecord/interface/JetResolutionRcd.h"
class TTree;
class Tupel : public edm::EDAnalyzer {

public:
  /// default constructor
  explicit Tupel(const edm::ParameterSet&);
  /// default destructor
  ~Tupel();

private:
/// everything that needs to be done before the event loop
  virtual void beginJob() ;
  /// everything that needs to be done during the event loop 
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
/// everything that needs to be done after the event loop
//   double getJER(double jetEta, int sysType);
  virtual void endJob() ;
  std::string channel_;
  bool keepparticlecoll_;
  std::string elecMatch_;
  std::string muonMatch_;
  std::string muonMatch2_;
  edm::EDGetTokenT<pat::Photon> photonSrc_;
  edm::EDGetTokenT<edm::View<pat::Electron> > electronToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > eleIdvetoToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > eleIdlooseToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > eleIdmediumToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > eleIdtightToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > eleHLTPreseToken_;
  edm::EDGetTokenT<pat::MuonCollection> muonSrc_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> pfcandSrc_;
  edm::EDGetTokenT<edm::View<pat::Jet> >jetToken_;
//  edm::EDGetTokenT<edm::ValueMap<double> > deepFJetTags_probudsgToken_;
  edm::EDGetTokenT<reco::GenJetCollection> gjetToken_;
//  edm::EDGetTokenT<pat::METCollection> metSrc_;
  std::vector<edm::EDGetTokenT<pat::METCollection> > metSources;
  edm::EDGetTokenT<double> mSrcRho_;
  edm::EDGetTokenT<LHEEventProduct> lheSource_;
  edm::EDGetTokenT<LHEEventProduct> lheEventToken;
  edm::EDGetTokenT<reco::GenParticleCollection> genParticleSrc_;
  edm::EDGetTokenT<pat::PackedGenParticleCollection> packedgenParticleSrc_;

  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
  edm::EDGetTokenT<reco::ConversionCollection> convToken_;
  edm::EDGetTokenT<GenEventInfoProduct> genInfoToken_;
  edm::EDGetTokenT<edm::TriggerResults> HLTToken_;
  edm::EDGetTokenT<edm::TriggerResults> HLTTokenFilters_;

  edm::EDGetTokenT<LHEEventProduct> lheEventSrc_;
  edm::EDGetTokenT<LHERunInfoProduct> lheRunInfoToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > PupSrc_;
  bool elecIdsListed_=false;
  //edm::EDGetTokenT<edm::ValueMap<float> > full5x5SigmaIEtaIEtaMapToken_;

  // ----------member data ---------------------------
  TTree *myTree;
  unsigned int event,run,lumi;
  int realdata,bxnumber;
  double EvtInfo_NumVtx,PU_npT,PU_npIT,nup,first_PV;
    double wtot_write=0;
    bool accept=false;
  //particles
  std::vector<double> Uncorec_METPt;
  std::vector<double> Uncorec_METPhi;
  std::vector<double> METPt;
  std::vector<double> METPx;
  std::vector<double> METPy;
  std::vector<double> METPz;
  std::vector<double> METE;
  std::vector<double> METsigx2;
  std::vector<double> METsigxy;
  std::vector<double> METsigy2;   
  std::vector<double> METsig;
  std::vector<double> METUncPtUp;
  std::vector<double> METUncPtDn;
  std::vector<double> METUncPhiUp;
  std::vector<double> METUncPhiDn;
  std::vector<double> Dr01LepPt;
  std::vector<double> Dr01LepEta;
  std::vector<double> Dr01LepPhi;
  std::vector<double> Dr01LepE;
  std::vector<double> Dr01LepM;
  std::vector<double> Dr01LepId;
  std::vector<double> Dr01LepStatus;
  std::vector<double> Dr01LepMomId;
  std::vector<bool> Dr01LepIsPrompt;
  std::vector<bool> Dr01LepIsTauProd;
  std::vector<double> Bare01LepPt;
  std::vector<double> Bare01LepEta;
  std::vector<double> Bare01LepPhi;
  std::vector<double> Bare01LepE;
  std::vector<double> Bare01LepM;
  std::vector<double> Bare01LepId;
  std::vector<double> Bare01LepStatus;
  std::vector<double> Bare01LepMomId;

  std::vector<double> Packed01Pt;
  std::vector<double> Packed01Eta;
  std::vector<double> Packed01Phi;
  std::vector<double> Packed01E;
  std::vector<double> Packed01M;
  std::vector<double> Packed01Id;
  std::vector<double> Packed01Status;
  std::vector<double> Packed01MomId;
  std::vector<double> Packed01Charge;
  std::vector<bool> Packed01IsPrompt;
  std::vector<bool> Packed01IsTauProd;

  std::vector<double> St03Pt;
  std::vector<double> St03Eta;
  std::vector<double> St03Phi;
  std::vector<double> St03E;
  std::vector<double> St03M;
  std::vector<double> St03Id;
  std::vector<double> St03Status;
  std::vector<double> St03MotherId;
  std::vector<double> St03NumberMom;
  std::vector<double> St01PhotonPt;
  std::vector<double> St01PhotonEta;
  std::vector<double> St01PhotonPhi;
  std::vector<double> St01PhotonE;
  std::vector<double> St01PhotonM;
  std::vector<double> St01PhotonId;
  std::vector<double> St01PhotonMomId;
  std::vector<double> St01PhotonNumberMom;
  std::vector<double> St01PhotonStatus;

  std::vector<bool> matchGjet;
  std::vector<double> MGjPt;
  std::vector<double> MGjeta;
  std::vector<double> MGjphi;
  std::vector<double> MGjE;
  //CaloJets
 /* std::vector<double> caloJetPt_;
  std::vector<double> caloJetRawPt_;
  std::vector<double> caloJetEn_;
  std::vector<double> caloJetEta_;
  std::vector<double> caloJetPhi_;
  std::vector<double> caloJetHadEHF_;
  std::vector<double> caloJetEmEHF_;
  std::vector<double> caloJetEmFrac_;
  std::vector<double> caloJetn90_;*/
  ///pfjets
  std::vector<double> patJetPfAk04PtJERSmear;
  std::vector<double> patJetPfAk04PtJERSmearUp;
  std::vector<double> patJetPfAk04PtJERSmearDn;
  std::vector<double> patJetPfAk04EnJERSmear;
  std::vector<double> patJetPfAk04EnJERSmearUp;
  std::vector<double> patJetPfAk04EnJERSmearDn;
  std::vector<double> patJetPfAk04En_;
  std::vector<double> patJetPfAk04Pt_;
  std::vector<double> patJetPfAk04Eta_;
  std::vector<double> patJetPfAk04Phi_;
  //std::vector<double> patJetPfAk04JesUncert_;
  std::vector<double> patJetPfAk04LooseId_;
  std::vector<double> patJetPfAk04Et_;
  std::vector<double> patJetPfAk04PartonFlavour_;
  std::vector<double> patJetPfAk04HadronFlavour_;
  std::vector<double> patJetPfAk04RawPt_;
  std::vector<double> patJetPfAk04RawEn_;
  std::vector<double> patJetPfAk04HadEHF_;
  std::vector<double> patJetPfAk04EmEHF_;
  std::vector<double> patJetPfAk04chf_;
  std::vector<double> patJetPfAk04nhf_;
  std::vector<double> patJetPfAk04cemf_;
  std::vector<double> patJetPfAk04nemf_;
  std::vector<double> patJetPfAk04muf_;
  std::vector<double> patJetPfAk04cmult_;
  std::vector<double> patJetPfAk04nconst_;
  std::vector<double> patJetPfAk04PtUp_;
  std::vector<double> patJetPfAk04PtDn_;
  std::vector<double> patJetPfAk04BDiscdeepFudsg_;
  std::vector<double> patJetPfAk04BDiscdeepFb_;
  std::vector<double> patJetPfAk04BDiscdeepFbb_;
  std::vector<double> patJetPfAk04BDiscdeepFc_;
  std::vector<double> patJetPfAk04BDiscdeepFcc_;
  std::vector<double> patJetPfAk04BDiscCSVv2_;
  std::vector<double> patJetPfAk04BDiscpfCMVA_;
  std::vector<double> patJetPfAk04BDiscCSVSLV1_;
  std::vector<double> unc_;
std::vector<double> patJetPfAk04ConstId;
std::vector<double> patJetPfAk04ConstPt;
std::vector<double> patJetPfAk04ConstCharge;
std::vector<double> patJetPfAk04ConstEta;
std::vector<double> patJetPfAk04ConstPhi;
std::vector<double> patJetPfAk04ConstE;

  ///Muons
  std::vector<double> patMuonPt_;
  std::vector<double> patMuonEta_;
  std::vector<double> patMuonPhi_;
  std::vector<double> patMuonVtxZ_;
  std::vector<double> patMuonEn_;
  std::vector<double> patMuonCharge_;
  std::vector<double> patMuonDxy_;
  std::vector<double> patMuonCombId_;
  std::vector<double> patMuonLooseId_;
  std::vector<double> patMuonMediumId_;
  std::vector<double> patMuonTightId_;
  std::vector<double> patMuonDetIsoRho_;
  std::vector<double> patMuonPfIsoDbeta_;
  std::vector<double> patMuonM_;
  std::vector<double> patMuonPx_;
  std::vector<double> patMuonPy_;
  std::vector<double> patMuonPz_;
  std::vector<double> patMuonGlobalType_;
  std::vector<double> patMuonTrackerType_;
  std::vector<double> patMuonPFType_;
  std::vector<double> patMuonIsoSumPt_;
  std::vector<double> patMuonIsoRelative_;
  std::vector<double> patMuonIsoCalComb_;
  std::vector<double> patMuonIsoDY_;
  std::vector<double> patMuonChi2Ndoff_;
  std::vector<double> patMuonNhits_;
  std::vector<double> patMuonNMatches_;
  std::vector<double> patMuonDz_;
  std::vector<double> patMuonPhits_;
  std::vector<double> patMuonTkLayers_;
  std::vector<double> patMuon_PF_IsoSumChargedHadronPt_;
  std::vector<double> patMuon_PF_IsoSumNeutralHadronEt_;
  std::vector<double> patMuon_PF_IsoDY_;
  std::vector<unsigned>  patElecIdveto_;
  std::vector<unsigned>  patElecIdloose_;
  std::vector<unsigned>  patElecIdmedium_;
  std::vector<unsigned>  patElecIdtight_;

    std::vector<double> patElecdEtaInSeed_;
    std::vector<double> patElecdPhiIn_;
    std::vector<double> patElechOverE_;
    std::vector<double> patElecsigmaIetaIeta_;
    std::vector<double> patElecfull5x5_sigmaIetaIeta_;
    std::vector<double> patElecooEmooP_;
    std::vector<double> patElecd0_;
    std::vector<double> patElecdz_;
    std::vector<int>   patElecexpectedMissingInnerHits_;
    std::vector<int>   patElecpassConversionVeto_;     
  std::vector<double> patElecDz_;
  std::vector<double> patElecPt_;
  std::vector<double> patElecEta_;
  std::vector<double> patElecScEta_;
  std::vector<double> patElecPhi_;
  std::vector<double> patElecEnergy_;
  std::vector<double> patElecCharge_;
  std::vector<double> patPfCandPt;
  std::vector<double> patPfCandEta;
  std::vector<double> patPfCandPhi;
  std::vector<double> patPfCandE;
  std::vector<double> patPfCandM;
  std::vector<double> patPfCandPx;
  std::vector<double> patPfCandPhiAtVtx;
  std::vector<double> patPfCandLostInnerHits;
  std::vector<double> patPfCandTrackHighPurity;
  std::vector<double> patPfCandPuppiWeight;
  std::vector<double> patPfCandPuppiWeightNolep;
std::vector<double> patPfCandCharge;
std::vector<double>  patPfCandDxy;
std::vector<double>  patPfCandDxyerr;
std::vector<double> patPfCandDz;
std::vector<double>  patPfCandDzerr;
std::vector<double>  patPfpvAssociationQuality;
std::vector<double> patPfCandDzAssociatedPV;
std::vector<double> patPfCandFromPv;
std::vector<double> patPfCandVertexRef;
       std::vector<double>  patElecchIso03_;
       std::vector<double>  patElecnhIso03_;
       std::vector<double>  patElecphIso03_;
       std::vector<double>  patElecpuChIso03_;
  std::vector<double> patElecPfIso_;
  std::vector<double> patElecPfIsodb_;
  std::vector<double> patElecPfIsoRho_;
  std::vector<unsigned>  patElecId_;
  std::vector<double> charged_;
  std::vector<double> patElec_mva_presel_;
  std::vector<double> id1_pdfInfo_;
  std::vector<double> id2_pdfInfo_;
  std::vector<double> x1_pdfInfo_;
  std::vector<double> x2_pdfInfo_;
  std::vector<double> scalePDF_pdfInfo_;
  double ptHat_,mcWeight_; 
  std::vector<double> mcWeights_;
  double rhoPrime,AEff;
  //HLT
    double HLT_IsoMu22_v;
    double HLT_IsoMu24;
    double HLT_IsoTkMu22_v;
    double HLT_IsoTkMu24;

  double HLT_Ele32_eta2p1_WPTight_Gsf; //TO BE USED
  double Flag_HBHENoiseFilter; //TO BE USED
  double Flag_HBHENoiseIsoFilter; //TO BE USED
  double Flag_globalTightHalo2016Filter; //TO BE USED
  double Flag_EcalDeadCellTriggerPrimitiveFilter; //TO BE USED
  double Flag_goodVertices; //TO BE USED
  double Flag_eeBadScFilter; //TO BE USED
  double Flag_chargedHadronTrackResolutionFilter; //do not use - those are under study 76X
  double Flag_muonBadTrackFilter; //do not use - those are under study for 76X

  JetCorrectionUncertainty *jecUnc;
};

using namespace std;
using namespace reco;
int ccnevent=0;
Tupel::Tupel(const edm::ParameterSet& iConfig)

{
//trigger_( iConfig.getParameter< edm::InputTag >( "trigger" ) ),
  //triggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  //triggerSummaryLabel_( iConfig.getParameter< edm::InputTag >( "triggerSummaryLabel" ) ), //added by jyhan
  channel_= iConfig.getParameter< std::string >( "channel" ) ;
keepparticlecoll_= iConfig.getParameter< bool >( "keepparticlecoll" ) ;
  elecMatch_= iConfig.getParameter< std::string >( "elecMatch" ) ;
  muonMatch_= iConfig.getParameter< std::string >( "muonMatch" ) ;
  muonMatch2_= iConfig.getParameter< std::string >( "muonMatch2" ) ;
//  ea_ . reset (new EffectiveAreas( edm::FileInPath(iConfig.getParameter<std::string>("effAreas_file")).fullPath () ) );
//effAreas_(edm::FileInPath(iConfig.getParameter< std::string >( "effAreas_file")).fullPath ());
//jecunctable_=iConfig.getParameter<std::string >("jecunctable");

  photonSrc_=consumes<pat::Photon>(iConfig.getParameter<edm::InputTag>("photonSrc"));
//  elecSrc_=consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronSrc"));
  electronToken_ =consumes<edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("electronSrc"));
  eleIdvetoToken_=consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("cutBasedElectronID_Summer16_80X_V1_veto"));
  eleIdlooseToken_=consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("cutBasedElectronID_Summer16_80X_V1_loose"));
  eleIdmediumToken_=consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("cutBasedElectronID_Summer16_80X_V1_medium"));
  eleIdtightToken_=consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("cutBasedElectronID_Summer16_80X_V1_tight"));
  eleHLTPreseToken_=consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("cutBasedElectronHLTPreselection_Summer16_V1"));
  muonSrc_=consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muonSrc"));
  pfcandSrc_=consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfcandSrc"));
  jetToken_=consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("jetSrc" ));
//  deepFJetTags_probudsgToken_=consumes<edm::ValueMap<double> >(iConfig.getParameter<edm::InputTag>("deepFlavourJetTags_probudsg"));
  gjetToken_=consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("gjetSrc" ));
//  metSrc_=consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metSrc"));
  mSrcRho_=consumes<double>(edm::InputTag("fixedGridRhoFastjetAll" ));//hardcode
  lheSource_=consumes<LHEEventProduct>(edm::InputTag ("externalLHEProducer"));//hardcode
  lheEventToken=consumes<LHEEventProduct>(edm::InputTag ("source"));


 // metSources(consumes<std::vector< pat::MET > >(iConfig.getParameter<std::vector<edm::InputTag> >("metSource"));

  genParticleSrc_=consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genSrc"));
  packedgenParticleSrc_=consumes<pat::PackedGenParticleCollection>(iConfig.getParameter<edm::InputTag>("pgenSrc"));


  beamSpotToken_=consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));//hardcode
//  vertexToken_=consumes<reco::VertexCollection>(edm::InputTag("goodOfflinePrimaryVertices"));//hardcode
  vertexToken_=consumes<reco::VertexCollection>(edm::InputTag("offlineSlimmedPrimaryVertices"));//hardcode
  convToken_=consumes<reco::ConversionCollection>(edm::InputTag("reducedEgamma","reducedConversions"));//hardcode
  genInfoToken_=consumes<GenEventInfoProduct>(edm::InputTag ("generator"));//hardcode
  HLTToken_=consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag > ("HLTSrc"));
  HLTTokenFilters_=consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerfilters"));
  lheEventSrc_=consumes<LHEEventProduct>(edm::InputTag ("externalLHEProducer"));//hardcode
  lheRunInfoToken_=consumes<LHERunInfoProduct>(edm::InputTag ("externalLHEProducer"));//hardcode
  PupSrc_=consumes<std::vector< PileupSummaryInfo> >(edm::InputTag ("slimmedAddPileupInfo"));//hardcode
 for (edm::InputTag const & tag : iConfig.getParameter< std::vector<edm::InputTag> > ("metSource"))metSources.push_back(consumes<pat::METCollection>(tag));
}

Tupel::~Tupel()
{
}

void Tupel::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  using namespace edm; //ADD
  ++ccnevent;
 
  // PAT trigger event
  //edm::Handle<pat::TriggerEvent>  triggerEvent;
  //iEvent.getByToken( triggerEvent_, triggerEvent );
  

 // edm::Handle<edm::ValueMap<float> > full5x5sieie;
 // iEvent.getByToken(full5x5SigmaIEtaIEtaMapToken_,full5x5sieie);


edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
iSetup.get<JetCorrectionsRecord>().get("AK4PFchs",JetCorParColl); 
JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"];
JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(JetCorPar);
  const pat::helper::TriggerMatchHelper matchHelper;	
  edm::Handle<GenParticleCollection> genParticles_h;
  iEvent.getByToken(genParticleSrc_, genParticles_h);
  const GenParticleCollection* genParticles  = genParticles_h.failedToGet () ? 0 : &*genParticles_h;

  edm::Handle<pat::PackedGenParticleCollection > packedgenParticles_h;
  iEvent.getByToken(packedgenParticleSrc_, packedgenParticles_h);
  const vector<pat::PackedGenParticle>* packedgenParticles  = packedgenParticles_h.failedToGet () ? 0 : &*packedgenParticles_h;

/*  edm::Handle<std::vector <reco::GenJet> >genjetColl;
  iEvent.getByToken(gjetToken_, genjetColl);
  std::vector<const reco::GenJet*> genjets_v;
  for ( auto& x : *genjetColl ) {
       genjets_v.push_back(&x);
       }
*/
  
  // get muon collection
  edm::Handle<pat::MuonCollection > muons;
  iEvent.getByToken(muonSrc_,muons);
  const std::vector<pat::Muon> * muon = muons.failedToGet () ? 0 : &*muons;

  
			      
 // edm::Handle<reco::GsfElectronCollection> els_h;
 // iEvent.getByToken("gsfElectrons", els_h);			       
  edm::Handle<reco::ConversionCollection> conversions_h;
  iEvent.getByToken(convToken_, conversions_h);
  
  
  
  // get tau collection 
//  edm::Handle<edm::View<pat::Tau> > taus;
//  iEvent.getByToken(tauSrc_,taus);
					

  edm::Handle<pat::PackedCandidateCollection > pfcand;
  iEvent.getByToken(pfcandSrc_,pfcand);
  const vector<pat::PackedCandidate> * pfcands = pfcand.failedToGet () ? 0 : &*pfcand ;
  
  // get met collection  
 // edm::Handle<edm::View<pat::MET> > mets;
//  iEvent.getByToken(metSrc_,mets);
  
  // get photon collection  
  edm::Handle<edm::View<pat::Photon> > photons;
  iEvent.getByToken(photonSrc_,photons);
  
  edm::Handle<reco::VertexCollection >  pvHandle;
  iEvent.getByToken(vertexToken_, pvHandle);
  reco::VertexCollection vtxx = *pvHandle;
			  
  edm ::Handle<reco::VertexCollection> vtx_h;
  iEvent.getByToken(vertexToken_, vtx_h); 
//  if(pvHandle.isValid()){
  int nvtx=vtx_h->size();
  if(vtx_h->empty()) return;
  const reco::Vertex &primVtx = vtx_h->front();
  reco::VertexRef primVtxRef(vtx_h,0);
  if(nvtx==0) return;
//  }
  edm::Handle<double>  rho_;
  iEvent.getByToken(mSrcRho_, rho_);
  double rhoIso=99;
  if(!rho_.failedToGet())rhoIso = *rho_;
  reco::BeamSpot beamSpot;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(beamSpotToken_, beamSpotHandle);
  if(!beamSpotHandle.failedToGet())beamSpot = *beamSpotHandle;
  edm::Handle<double> rho;
  iEvent.getByToken(mSrcRho_,rho);

patPfCandPt.clear();
patPfCandEta.clear();
patPfCandPhi.clear();
patPfCandE.clear();
patPfCandM.clear();
patPfCandPx.clear();
patPfCandPhiAtVtx.clear();
patPfCandLostInnerHits.clear();
patPfCandTrackHighPurity.clear();
patPfCandPuppiWeight.clear();
patPfCandPuppiWeightNolep.clear();
      patPfCandCharge.clear();
       patPfCandDxy.clear();
       patPfCandDxyerr.clear();
      patPfCandDz.clear();
patPfCandDzAssociatedPV.clear();
       patPfCandDzerr.clear();
patPfpvAssociationQuality.clear();
patPfCandFromPv.clear();
patPfCandVertexRef.clear();
    event=0;
    realdata=0;
    run=0;
    lumi=0;
    bxnumber=0;
    EvtInfo_NumVtx=0;
    PU_npT=0;
    PU_npIT=0;
    nup=0;
    rhoPrime=0;
    AEff=0; 
    Uncorec_METPt.clear();
    Uncorec_METPhi.clear();
    METPt.clear();
    METPx.clear();
    METPy.clear();
    METPz.clear();
    METE.clear();
    METsigx2.clear();
    METsigxy.clear();
    METsigy2.clear();
    METsig.clear();   
    METUncPtUp.clear();   
    METUncPtDn.clear();   
    METUncPhiUp.clear();   
    METUncPhiDn.clear();   
    Dr01LepPt.clear();
    Dr01LepEta.clear();
    Dr01LepPhi.clear();
    Dr01LepE.clear();
    Dr01LepM.clear();
    Dr01LepId.clear();
    Dr01LepStatus.clear();
  Dr01LepMomId.clear();

          Dr01LepIsPrompt.clear();
          Dr01LepIsTauProd.clear();
    Bare01LepPt.clear();
    Bare01LepEta.clear();
    Bare01LepPhi.clear();
    Bare01LepE.clear();
    Bare01LepM.clear();
    Bare01LepId.clear();
    Bare01LepStatus.clear();
    Bare01LepMomId.clear();
    Packed01Pt.clear();
    Packed01Eta.clear();
    Packed01Phi.clear();
    Packed01E.clear();
    Packed01M.clear();
    Packed01Id.clear();
    Packed01Status.clear();
  Packed01MomId.clear();
Packed01Charge.clear();
         Packed01IsPrompt.clear();
          Packed01IsTauProd.clear();

    St03Pt.clear();
    St03Eta.clear();
    St03Phi.clear();
    St03E.clear();
    St03M.clear();
    St03Id.clear();
    St03Status.clear();
 St03MotherId.clear();
St03NumberMom.clear();
    St01PhotonPt.clear();
    St01PhotonEta.clear();
    St01PhotonPhi.clear();
    St01PhotonE.clear();
    St01PhotonM.clear();
    St01PhotonId.clear();
    St01PhotonMomId.clear();
    St01PhotonNumberMom.clear();
    St01PhotonStatus.clear();
    matchGjet.clear();
    MGjPt.clear();
    MGjeta.clear();
    MGjphi.clear();
    MGjE.clear();
    patJetPfAk04PtJERSmear.clear();
    patJetPfAk04PtJERSmearUp.clear();
    patJetPfAk04PtJERSmearDn.clear();
    patJetPfAk04EnJERSmear.clear();
    patJetPfAk04EnJERSmearUp.clear();
    patJetPfAk04EnJERSmearDn.clear();
    patJetPfAk04En_.clear();
    patJetPfAk04Pt_.clear();
    patJetPfAk04Eta_.clear();
    patJetPfAk04Phi_.clear();
    //patJetPfAk04JesUncert_.clear();
    patJetPfAk04LooseId_.clear();
    patJetPfAk04Et_.clear();
    patJetPfAk04PartonFlavour_.clear();
    patJetPfAk04HadronFlavour_.clear();
    patJetPfAk04RawPt_.clear();
    patJetPfAk04RawEn_.clear();
    patJetPfAk04HadEHF_.clear();
    patJetPfAk04EmEHF_.clear();
    patJetPfAk04chf_.clear();
    patJetPfAk04nhf_.clear();
    patJetPfAk04cemf_.clear();
    patJetPfAk04nemf_.clear();
    patJetPfAk04muf_.clear();
    patJetPfAk04cmult_.clear();
    patJetPfAk04nconst_.clear();
    patJetPfAk04PtUp_.clear();
    patJetPfAk04PtDn_.clear();
    patJetPfAk04BDiscdeepFudsg_.clear();
    patJetPfAk04BDiscdeepFb_.clear();
    patJetPfAk04BDiscdeepFbb_.clear();
    patJetPfAk04BDiscdeepFc_.clear();
    patJetPfAk04BDiscdeepFcc_.clear();
    patJetPfAk04BDiscCSVv2_.clear();
    patJetPfAk04BDiscpfCMVA_.clear();
    patJetPfAk04BDiscCSVSLV1_.clear();
      patJetPfAk04ConstId.clear();
      patJetPfAk04ConstPt.clear();
      patJetPfAk04ConstCharge.clear();
      patJetPfAk04ConstEta.clear();
      patJetPfAk04ConstPhi.clear();
      patJetPfAk04ConstE.clear();
    unc_.clear();
    ///Muons
    patMuonPt_.clear();
    patMuonEta_.clear();
    patMuonPhi_.clear();
    patMuonVtxZ_.clear();
    patMuonEn_.clear();
    patMuonCharge_.clear();
    patMuonDxy_.clear();
    patMuonCombId_.clear();
    patMuonLooseId_.clear();
    patMuonMediumId_.clear();
    patMuonTightId_.clear();
    patMuonDetIsoRho_.clear();
    patMuonPfIsoDbeta_.clear();
    patMuonM_.clear();
    patMuonPx_.clear();
    patMuonPy_.clear();
    patMuonPz_.clear();
    patMuonGlobalType_.clear();
    patMuonTrackerType_.clear();
    patMuonPFType_.clear();
    patMuonIsoSumPt_.clear();
    patMuonIsoRelative_.clear();
    patMuonIsoCalComb_.clear();
    patMuonIsoDY_.clear();
    patMuonChi2Ndoff_.clear();
    patMuonNhits_.clear();
    patMuonNMatches_.clear();
    patMuonDz_.clear();
    patMuonPhits_.clear();
    patMuonTkLayers_.clear();
    patMuon_PF_IsoSumChargedHadronPt_.clear();
    patMuon_PF_IsoSumNeutralHadronEt_.clear();
    patMuon_PF_IsoDY_.clear();
    //electrons
    patElecIdveto_.clear();
    patElecIdloose_.clear();
    patElecIdmedium_.clear();
    patElecIdtight_.clear();
    patElecdEtaInSeed_.clear();
    patElecdPhiIn_.clear();
    patElechOverE_.clear();
    patElecsigmaIetaIeta_.clear();
    patElecfull5x5_sigmaIetaIeta_.clear();
    patElecooEmooP_.clear();
    patElecd0_.clear();
    patElecdz_.clear();
    patElecexpectedMissingInnerHits_.clear();
    patElecpassConversionVeto_.clear();     
    patElecDz_.clear();
    patElecPt_.clear();
    patElecEta_.clear();
    patElecScEta_.clear();
    patElecPhi_.clear();
    patElecEnergy_.clear();
    patElecCharge_.clear();
    patElecchIso03_.clear();
    patElecnhIso03_.clear();
    patElecphIso03_.clear();
    patElecpuChIso03_.clear();
    patElecPfIso_.clear();
    patElecPfIsodb_.clear();
    patElecPfIsoRho_.clear();
    patElecId_.clear();
    charged_.clear();
    patElec_mva_presel_.clear();
    //HLT 
    
    HLT_Ele32_eta2p1_WPTight_Gsf=0;
      HLT_IsoMu22_v=0;
      HLT_IsoMu24=0;
      HLT_IsoTkMu22_v=0;
      HLT_IsoTkMu24=0;

    ////Add 08/23/13/////
    id1_pdfInfo_.clear();
    id2_pdfInfo_.clear();
    x1_pdfInfo_.clear();
    x2_pdfInfo_.clear();
    scalePDF_pdfInfo_.clear();
    ptHat_=0;
    mcWeight_=0;
    mcWeights_.clear();
    ///////////////////end clear vector////////////////////// 
    event = iEvent.id().event();
    run = iEvent.id().run();
    lumi = iEvent.luminosityBlock();
    bxnumber = iEvent.bunchCrossing();
    realdata = iEvent.isRealData();
    

     for(unsigned int imet=0;imet<metSources.size();imet++){
        Handle<pat::METCollection> metH;
        iEvent.getByToken(metSources[imet], metH);
        if(!metH.isValid())continue;
	using metUnc = pat::MET::METUncertainty;
        METUncPtUp.push_back(metH->at(0).shiftedPt(metUnc::UnclusteredEnUp, pat::MET::Type1));
        METUncPtDn.push_back(metH->at(0).shiftedPt(metUnc::UnclusteredEnDown, pat::MET::Type1));
        METUncPhiUp.push_back(metH->at(0).shiftedPhi(metUnc::UnclusteredEnUp, pat::MET::Type1));
        METUncPhiDn.push_back(metH->at(0).shiftedPhi(metUnc::UnclusteredEnDown, pat::MET::Type1));

        Uncorec_METPt.push_back(metH->at(0).uncorPt()); 
        Uncorec_METPhi.push_back(metH->at(0).uncorPhi()); 
        METPt.push_back(metH->at(0).pt()); 
        METPx.push_back(metH->at(0).px()); 
        METPy.push_back(metH->at(0).py()); 
        METPz.push_back(metH->at(0).pz()); 
        METE.push_back(metH->at(0).energy()); 
        METsigx2.push_back(metH->at(0).getSignificanceMatrix()(0,0)); 
        METsigxy.push_back(metH->at(0).getSignificanceMatrix()(0,1)); 
        METsigy2.push_back(metH->at(0).getSignificanceMatrix()(1,1)); 
        METsig.push_back(metH->at(0).significance()); 
//https://github.com/BristolTopGroup/NTupleProduction/blob/master/src/BristolNTuple_MET.cc
//        METUncPt.push_back(metH->at(0).shiftedPt(pat::MET::METUncertainty(1) )); 
//	std::cout<<"this is met pt unc: "<<METUncPt.at(0)<<endl;
        //Output object in EDM format
        //std::auto_ptr<llvvMet> metOut(new llvvMet());
        //llvvMet& met = *metOut;

        //////////////////////////////////

         // met.SetPxPyPzE(metH->ptrAt(0)->px(), metH->ptrAt(0)->py(), metH->ptrAt(0)->pz(), metH->ptrAt(0)->energy());
          //met.sigx2 = metH->ptrAt(0)->getSignificanceMatrix()(0,0);
          //met.sigxy = metH->ptrAt(0)->getSignificanceMatrix()(0,1);
          //met.sigy2 = metH->ptrAt(0)->getSignificanceMatrix()(1,1);
          //met.sig   = metH->ptrAt(0)->significance();

         //iEvent.put(metOut, metSources[imet].label()); //save the object to the event here, to keep it in the loop
       }




    if(pfcands){
    //  int cout_ind=0;
      for( unsigned int i=0; i<pfcands->size(); ++i){
        const pat::PackedCandidate & pf = pfcands->at(i);
   //    if(pf.charge()==0 || pf.pt()<0.3 ||(fabs(pf.dzError())!=0&&fabs(pf.dzAssociatedPV()/pf.dzError())>10) ||  (fabs(pf.dxyError())!=0&&fabs(pf.dxy()/pf.dxyError())>10) ||fabs(pf.dzError())==0||fabs(pf.dxyError())==0) continue;
       if(pf.charge()==0 || pf.pt()<0.3 ||pf.vertexRef().key()!=0||fabs(pf.eta())>3.0) continue;
       //if(pf.charge()!=0){cout<<pf.charge()<<"  "<<pf.dzError()<<"  "<<pf.dz()<<"  "<<" "<<pf.vx()<<"  "<<pf.vz()<<"  "<<endl;}
       //cout<< pf.fromPV()<<endl;   
       double frompv;
	  //if( !pvHandle->empty() && !pvHandle->front().isFake() ) {
	  //  const reco::Vertex &vtx = pvHandle->front();
 //           cout<<vtx.position()<<"  "<<pvHandle->at(0).position()<<"  "<<pf.vertexRef().key()<<endl;
	    frompv= pf.fromPV();
      //      cout_ind=1;

	  //}   
       patPfCandVertexRef.push_back(pf.vertexRef().key());
       patPfCandFromPv.push_back(frompv);
    //   if(pf.dxyError()==0||pf.dzError()==0)cout<<pf.dxy()<<"  "<<pf.dxyError()<<"  "<<pf.dz()<<"  "<<pf.dzError()<<"                 "<<frompv<<"  "<<pf.vertexRef().key()<<"  "<<pf.pvAssociationQuality()<<endl;
       patPfCandPt.push_back(pf.pt());
       patPfCandCharge.push_back(pf.charge());
       patPfCandDxy.push_back(pf.dxy());
       patPfCandDxyerr.push_back(pf.dxyError());
      patPfCandDz.push_back(pf.dz());
      patPfCandDzAssociatedPV.push_back(pf.dzAssociatedPV());
       patPfCandDzerr.push_back(pf.dzError());
       //cout<<pf.pvAssociationQuality()<<endl;
       patPfpvAssociationQuality.push_back(pf.pvAssociationQuality());
       patPfCandEta.push_back(pf.eta());
       patPfCandPhi.push_back(pf.phi());
       patPfCandE.push_back(pf.energy());
       patPfCandM.push_back(pf.mass());
       patPfCandPx.push_back(pf.px());
       patPfCandPhiAtVtx.push_back(pf.phiAtVtx());
       patPfCandLostInnerHits.push_back(pf.lostInnerHits());
       patPfCandTrackHighPurity.push_back(pf.trackHighPurity());
       patPfCandPuppiWeight.push_back(pf.puppiWeight());
       patPfCandPuppiWeightNolep.push_back(pf.puppiWeightNoLep()); 
 
      }
 
    }


//    EvtInfo_NumVtx = 0;
    first_PV=0;
    std::vector<bool>isfake;std::vector<double>z_value;std::vector<double>rho_vlaue;std::vector<double>ndof_value;
    if(pvHandle.isValid()){
    for (reco::VertexCollection::const_iterator vtx = vtxx.begin(); vtx !=vtxx.end(); ++vtx){
      if (vtx->isValid() ){
//if (run==1 && lumi==39870 && event==7941180)cout<<"run:lumi:event, "<<run<<":"<<lumi<<":"<<event<< ",  isFake = "<<vtx->isFake()<<", z =  "<<vtx->z()<<", rho= "<<vtx->position().Rho()<<", ndof= "<<vtx->ndof()<<endl;
      isfake.push_back(vtx->isFake());z_value.push_back(vtx->z());
      rho_vlaue.push_back(vtx->position().Rho());ndof_value.push_back(vtx->ndof());
      ++EvtInfo_NumVtx ;
     }
    }
   }
if (isfake.at(0)=1 && fabs(z_value.at(0))< 24.0 && rho_vlaue.at(0) < 2. && ndof_value.at(0)>= 4.) first_PV=1;
    if(!realdata){
      Handle<std::vector< PileupSummaryInfo > >  PupInfo;
      iEvent.getByToken(PupSrc_, PupInfo);
      if(!PupInfo.failedToGet()){
      std::vector<PileupSummaryInfo>::const_iterator PVI;
      float npT=-1.;
      float npIT=-1.;
      
      for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
	
	int BX = PVI->getBunchCrossing();
	
	if(BX == 0) {
	  npT = PVI->getTrueNumInteractions();
	  npIT = PVI->getPU_NumInteractions();
	}
      }
      PU_npT=npT;
      PU_npIT=npIT;
    }
    else {
      PU_npT=-2.; 
      PU_npIT=-2.;
    }
    }
    if (!realdata && genParticles){     
    const std::vector<reco::GenParticle> & gen = *genParticles_h;
    for (size_t i=0; i<genParticles->size(); ++i){
      TLorentzVector genR1DressLep1(0,0,0,0);
//      TLorentzVector genPho(0,0,0,0); 
      int st = gen[i].status();
      int id = gen[i].pdgId();

      if(gen[i].numberOfMothers()){
 
        if (abs(st)==23 ||abs(st)==22||abs(st)==21||abs(st)==61 ||abs(st)==3||abs(st)==2 ){
          TLorentzVector genLep3(0,0,0,0);

          if(abs(gen[i].eta())<10)genLep3.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
          if(abs(gen[i].eta())>10)genLep3.SetPxPyPzE(0.001,0.001,gen[i].pz(),gen[i].energy());
	  St03Pt.push_back(genLep3.Pt());
	  St03Eta.push_back(genLep3.Eta());
	  St03Phi.push_back(genLep3.Phi());
	  St03E.push_back(genLep3.Energy());
	  St03M.push_back(genLep3.M());
          St03MotherId.push_back(gen[i].mother()->pdgId());
	  St03NumberMom.push_back(gen[i].numberOfMothers());
	  St03Id.push_back(id);
	  St03Status.push_back(st);
        }

        /*if(gen[i].numberOfMothers() ==1 && gen[i].mother()->pdgId() != id){

         continue;
        }*/
        if (st==1 && (abs(id)==13||abs(id)==11 || abs(id)==15 ||abs(id)==12||abs(id)==14||abs(id)==16) /*&& gen[i].pt() > 0.1 && fabs(gen[i].eta())<3.0*/){

          TLorentzVector genLep1(0,0,0,0);
          genLep1.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
	  TLorentzVector genR1Pho1(0,0,0,0);

       	  edm::Handle<std::vector<reco::GenParticle> > genpart2;//DONT know why we Need to handle another collection
          iEvent.getByToken(genParticleSrc_, genpart2);
          const std::vector<reco::GenParticle> & gen2 = *genpart2;
            //LOOP over photons//
	  if (st==1 && (abs(id)==13||abs(id)==11)){
            for(unsigned int j=0; j<genpart2->size(); ++j){
              if(gen2[j].numberOfMothers()){
	        if( gen2[j].status()!=1|| gen2[j].pdgId()!=22 || gen2[j].energy()<0.000001 /*|| fabs(MomId2)!=fabs(id)*/) continue;
	        TLorentzVector thisPho1(0,0,0,0);
	        thisPho1.SetPtEtaPhiE(gen2[j].pt(),gen2[j].eta(),gen2[j].phi(),gen2[j].energy());
  	        double dR = genLep1.DeltaR(thisPho1);
	        if(dR<0.1){
	          genR1Pho1+=thisPho1;
	        }
	        if(dR<0.2){
	          St01PhotonPt.push_back(thisPho1.Pt());
	          St01PhotonEta.push_back(thisPho1.Eta());
	          St01PhotonPhi.push_back(thisPho1.Phi());
	          St01PhotonE.push_back(thisPho1.Energy());
	          St01PhotonM.push_back(thisPho1.M());
	          St01PhotonId.push_back(gen2[j].pdgId());
	          St01PhotonMomId.push_back(fabs(gen2[j].mother()->pdgId()));
	          St01PhotonNumberMom.push_back(gen2[j].numberOfMothers());
	          St01PhotonStatus.push_back(gen2[j].status());
	        }
              }
            }
          }
          genR1DressLep1=genLep1+genR1Pho1;
	  Dr01LepPt.push_back(genR1DressLep1.Pt());
	  Dr01LepEta.push_back(genR1DressLep1.Eta());
	  Dr01LepPhi.push_back(genR1DressLep1.Phi());
	  Dr01LepE.push_back(genR1DressLep1.Energy());
	  Dr01LepM.push_back(genR1DressLep1.M());
	  Dr01LepId.push_back(id);
	  Dr01LepMomId.push_back(id);
	  Dr01LepStatus.push_back(st);

bool isPrompt = gen[i].isPromptFinalState();
	bool tauProd  = gen[i].isDirectPromptTauDecayProductFinalState();
          Dr01LepIsPrompt.push_back(isPrompt);
          Dr01LepIsTauProd.push_back(tauProd);
	  Bare01LepPt.push_back(genLep1.Pt());
	  Bare01LepEta.push_back(genLep1.Eta());
	  Bare01LepPhi.push_back(genLep1.Phi());
	  Bare01LepE.push_back(genLep1.Energy());
	  Bare01LepM.push_back(genLep1.M());
	  Bare01LepId.push_back(id);
	  Bare01LepMomId.push_back(id);
	  Bare01LepStatus.push_back(st);
        }
      }
    }
    }

bool gendielec=false;
bool genemu=false;
bool gendimu=false;
bool gensingleelec=false;
bool gensinglemu=false;
int ngjets=0;
    if (!realdata && packedgenParticles){  


      const vector<pat::PackedGenParticle> & gen = *packedgenParticles_h;
      for (size_t i=0; i<packedgenParticles->size(); ++i){

      int st = gen[i].status();
      int id = gen[i].pdgId();
   //  cout<<st<<"  "<<id<<"  "<<gen[i].pt()<<"  "<<gen[i].eta()<<"  "<<gen[i].phi()<<"  "<<gen[i].energy()<<endl;
          TLorentzVector genLep1(0,0,0,0);
          if(fabs(gen[i].eta())>0.00000001)genLep1.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
          else genLep1.SetPtEtaPhiE(0.00000001,0.00000001,gen[i].phi(),gen[i].energy());
          if( gen[i].pt()<0.3 || fabs(gen[i].eta())>3.||gen[i].charge()==0) continue;
          bool isPrompt = gen[i].isPromptFinalState();
          bool tauProd  = gen[i].isDirectPromptTauDecayProductFinalState();
          Packed01IsPrompt.push_back(isPrompt);
          Packed01IsTauProd.push_back(tauProd);
	  Packed01Pt.push_back(genLep1.Pt());
	  Packed01Eta.push_back(genLep1.Eta());
	  Packed01Phi.push_back(genLep1.Phi());
	  Packed01E.push_back(genLep1.Energy());
	  Packed01M.push_back(genLep1.M());
	  Packed01Id.push_back(id);
	  Packed01MomId.push_back(id);
	  Packed01Status.push_back(st);
          Packed01Charge.push_back(gen[i].charge());
      }
    }


    //cout<<"aaaaaa"<<endl;

    if (!realdata){
      //matrix element info
      Handle<LHEEventProduct> lheH;
      iEvent.getByToken(lheSource_,lheH);//to be modularized!!!
      if(lheH.isValid()) nup=lheH->hepeup().NUP;
    }
    
    //cout<<"bbbbbbbbb"<<endl;
    ////Add 08/27/13//////
      double w=1;

    if(!realdata){
      edm::Handle<GenEventInfoProduct>   genEventInfoProd;
      if (iEvent.getByToken(genInfoToken_,genEventInfoProd)) {
	if (genEventInfoProd->hasBinningValues())
	  ptHat_ = genEventInfoProd->binningValues()[0];
	mcWeight_ = genEventInfoProd->weight();
      }
      /// now get the PDF information
      edm::Handle<GenEventInfoProduct> pdfInfoHandle;
      if (iEvent.getByToken(genInfoToken_, pdfInfoHandle)) {
	if (pdfInfoHandle->pdf()) {
	  id1_pdfInfo_.push_back(pdfInfoHandle->pdf()->id.first);
	  id2_pdfInfo_.push_back(pdfInfoHandle->pdf()->id.second);
	  x1_pdfInfo_.push_back(pdfInfoHandle->pdf()->x.first);
	  x2_pdfInfo_.push_back(pdfInfoHandle->pdf()->x.second);
	  //pdfInfo_.push_back(pdfInfoHandle->pdf()->xPDF.first);
	  //dfInfo_.push_back(pdfInfoHandle->pdf()->xPDF.second);
	  scalePDF_pdfInfo_.push_back(pdfInfoHandle->pdf()->scalePDF);
	}   
      } 
/*      edm::Handle<LHEEventProduct>   lheEventProdH;
      iEvent.getByToken(lheEventToken, lheEventProdH);
      if (iEvent.getByToken(lheEventToken, lheEventProdH)){
      lheSigEvn=lheEventProdH->hepeup().IDPRUP;
	}*/
      edm::Handle<LHEEventProduct>   lheEventInfoProd;
      if (iEvent.getByToken(lheEventSrc_,lheEventInfoProd)) {
        //mcWeights_ = genEventInfoProd->weights();
              if(lheEventInfoProd->weights().size()>0)w=lheEventInfoProd->weights()[0].wgt;
        for(unsigned int size=0;size<lheEventInfoProd->weights().size();size++){
          mcWeights_.push_back(lheEventInfoProd->weights()[size].wgt);
	}
      }
/*
edm::Handle<LHERunInfoProduct> lheRunInfoH_; 
//if(iEvent.getByToken( lheRunInfoToken_, lheRunInfoH_ )) {
iEvent.getByLabel("externalLHEProducer", lheRunInfoH_);
//LHERunInfoProduct myLHERunInfoProduct = *(lheRunInfoH_.product());
//for(unsigned int iter=0;iter<lheRunInfoH_->size();iter++){
for (auto iter=lheRunInfoH_->headers_begin(); iter!=lheRunInfoH_->headers_end(); iter++){
cout<<"I am here:  "<<endl;
//  std::cout << iter->tag() << std::endl;
  std::vector<std::string> lines = iter->lines();
  for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
   std::cout << lines.at(iLine);
  }
}*/


  
    }

        //cout<<"ccccccccccccccc"<<endl;
    int Ele32_eta2p1_WPTight_Gsf=0;
    int IsoMu22_v=0;
    int IsoMu24=0;
    int IsoTkMu22_v=0;
    int IsoTkMu24=0;
    int ntrigs;
    vector<string> trigname;
    vector<bool> trigaccept;
    edm::Handle< edm::TriggerResults > HLTResHandle;
    //edm::InputTag HLTTag = edm::InputTag( "TriggerResults", "", "HLT");
    iEvent.getByToken(HLTToken_, HLTResHandle);
    if ( HLTResHandle.isValid() && !HLTResHandle.failedToGet() ) {
      edm::RefProd<edm::TriggerNames> trigNames( &(iEvent.triggerNames( *HLTResHandle )) );
      ntrigs = (int)trigNames->size();
      for (int i = 0; i < ntrigs; i++) {
	trigname.push_back(trigNames->triggerName(i));
	trigaccept.push_back(HLTResHandle->accept(i));
	if (trigaccept[i]){
if (run==278969 && lumi==1128&& event==1934843069){cout<<"trigname[i]:  "<<trigname[i]<<endl;}
//cout<<"this is trigger:  "<<trigname[i]<<endl;
	  if(std::string(trigname[i]).find("HLT_Ele32_eta2p1_WPTight_Gsf")!=std::string::npos){ Ele32_eta2p1_WPTight_Gsf=1;
}
	  if(std::string(trigname[i]).find("HLT_IsoMu22_v")!=std::string::npos){IsoMu22_v=1;
}
	  if(std::string(trigname[i]).find("HLT_IsoMu24")!=std::string::npos){IsoMu24=1;
}
	  if(std::string(trigname[i]).find("HLT_IsoTkMu22_v")!=std::string::npos)IsoTkMu22_v=1;
	  if(std::string(trigname[i]).find("HLT_IsoTkMu24")!=std::string::npos)IsoTkMu24=1;
	}
      }
    }

      HLT_Ele32_eta2p1_WPTight_Gsf=Ele32_eta2p1_WPTight_Gsf;
      HLT_IsoMu22_v=IsoMu22_v;
      HLT_IsoMu24=IsoMu24;
      HLT_IsoTkMu22_v=IsoTkMu22_v;
      HLT_IsoTkMu24=IsoTkMu24;

    edm::Handle< edm::TriggerResults > HLTResFiltersHandle;
    //edm::InputTag HLTTag = edm::InputTag( "TriggerResults", "", "HLT");
    iEvent.getByToken(HLTTokenFilters_, HLTResFiltersHandle);
    Flag_HBHENoiseFilter=0;
    Flag_HBHENoiseIsoFilter=0;
    Flag_globalTightHalo2016Filter=0;
    Flag_EcalDeadCellTriggerPrimitiveFilter=0;
    Flag_goodVertices=0;
    Flag_eeBadScFilter=0;
    Flag_chargedHadronTrackResolutionFilter =0;
    Flag_muonBadTrackFilter =0;

    if ( HLTResFiltersHandle.isValid() && !HLTResFiltersHandle.failedToGet() ) {
      edm::RefProd<edm::TriggerNames> trigNames( &(iEvent.triggerNames( *HLTResFiltersHandle )) );
      ntrigs = (int)trigNames->size();
      for (int i = 0; i < ntrigs; i++) {
        if(HLTResFiltersHandle->accept(i)){
          if(std::string(trigNames->triggerName(i)).find("Flag_HBHENoiseFilter")!=std::string::npos)Flag_HBHENoiseFilter=1.;
          if(std::string(trigNames->triggerName(i)).find("Flag_HBHENoiseIsoFilter")!=std::string::npos)Flag_HBHENoiseIsoFilter=1.;
          if(std::string(trigNames->triggerName(i)).find("Flag_globalTightHalo2016Filter")!=std::string::npos)Flag_globalTightHalo2016Filter=1.;
          if(std::string(trigNames->triggerName(i)).find("Flag_EcalDeadCellTriggerPrimitiveFilter")!=std::string::npos)Flag_EcalDeadCellTriggerPrimitiveFilter=1.;
          if(std::string(trigNames->triggerName(i)).find("Flag_goodVertices")!=std::string::npos)Flag_goodVertices=1.;
          if(std::string(trigNames->triggerName(i)).find("Flag_eeBadScFilter")!=std::string::npos)Flag_eeBadScFilter=1.;
          if(std::string(trigNames->triggerName(i)).find("Flag_chargedHadronTrackResolutionFilter")!=std::string::npos)Flag_chargedHadronTrackResolutionFilter=1.; 
          if(std::string(trigNames->triggerName(i)).find("Flag_muonBadTrackFilter")!=std::string::npos)Flag_muonBadTrackFilter=1.; 
        }
      }
    }
    double MuFill=0;
        //cout<<"dddddddddddddddddd"<<endl;
    if(muon){
    for (unsigned int j = 0; j < muons->size(); ++j){
      const std::vector<pat::Muon> & mu = *muons;
//     if(recomu[j].isGlobalMuon()){ 
        if((channel_!="noselection")&&(mu[j].pt()<15 || abs(mu[j].eta())>2.4))continue;
        if((channel_=="noselection")&&(mu[j].pt()<10 || abs(mu[j].eta())>3.0))continue;
	//MuFill=1;
	patMuonPt_.push_back(mu[j].pt());
	patMuonEta_.push_back(mu[j].eta());
	patMuonPhi_.push_back(mu[j].phi());
	patMuonEn_.push_back(mu[j].energy());
	patMuonCharge_.push_back(mu[j].charge());
	patMuonVtxZ_.push_back(mu[j].vz());
	patMuonM_.push_back(mu[j].mass());
	patMuonPx_.push_back(mu[j].px());
	patMuonPy_.push_back(mu[j].py());
	patMuonPz_.push_back(mu[j].pz());
	patMuonDxy_.push_back(mu[j].dB());
	
	double trkLayers = -99999;
	double pixelHits = -99999;
	double muonHits  = -99999;
	double nMatches  = -99999;
	double normChi2  = +99999;
	double dZ = -99999;
	bool isTrackMuon =mu[j].isTrackerMuon();
	bool isGlobalMuon =mu[j].isGlobalMuon();
	if(isTrackMuon && isGlobalMuon){
	  trkLayers     = mu[j].innerTrack()->hitPattern().trackerLayersWithMeasurement();
	  pixelHits     = mu[j].innerTrack()->hitPattern().numberOfValidPixelHits();
	  muonHits      = mu[j].globalTrack()->hitPattern().numberOfValidMuonHits();
	  nMatches      = mu[j].numberOfMatchedStations();
	  normChi2      = mu[j].globalTrack()->normalizedChi2();
	  if( !pvHandle->empty() && !pvHandle->front().isFake() ) {
	    const reco::Vertex &vtx = pvHandle->front();
	    dZ= mu[j].muonBestTrack()->dz(vtx.position());

	  }
	}
	patMuonChi2Ndoff_.push_back(normChi2);
	patMuonNhits_.push_back(muonHits);
	patMuonNMatches_.push_back(nMatches);
	patMuonDz_.push_back(dZ);
	patMuonPhits_.push_back(pixelHits);
	patMuonTkLayers_.push_back(trkLayers);
	
	int idpass=0;
        int tightid=0, medid=0, looseid=0;
  bool isTight=0;
  if(!( pvHandle->empty() && pvHandle->front().isFake())){
     const reco::Vertex &vtx = pvHandle->front();
     isTight=muon::isTightMuon(mu[j],vtx);
     }
     bool isMedium(muon::isMediumMuon(mu[j]));
     bool isLoose(muon::isLooseMuon(mu[j]));
     if (isTight)tightid=1;if (isMedium)medid=1;if (isLoose)looseid=1;

        patMuonLooseId_.push_back(looseid);
        patMuonMediumId_.push_back(medid);
        patMuonTightId_.push_back(tightid);
	if(mu[j].isGlobalMuon() && mu[j].isPFMuon() && normChi2<10 && muonHits>0 && nMatches>1 && mu[j].dB()<0.2 && dZ<0.5 && pixelHits>0 && trkLayers>5)idpass=1;
	patMuonCombId_.push_back(idpass);
	float muEta = mu[j].eta(); // essentially track direction at Vtx (recommended prescription)
	float Aecal=0.041; // initiallize with EE value
	float Ahcal=0.032; // initiallize with HE value
	if (fabs(muEta)<1.48) {
	  Aecal = 0.074;   // substitute EB value 
	  Ahcal = 0.023;   // substitute EE value
	} 
	double theRho = *rho;
	float muonIsoRho = mu[j].isolationR03().sumPt + std::max(0.,(mu[j].isolationR03().emEt -Aecal*(theRho))) + std::max(0.,(mu[j].isolationR03().hadEt-Ahcal*(theRho)));
	double dbeta = muonIsoRho/mu[j].pt();
	patMuonDetIsoRho_.push_back(dbeta);
	
	// pf Isolation variables
	double chargedHadronIso = mu[j].pfIsolationR04().sumChargedHadronPt;
	double chargedHadronIsoPU = mu[j].pfIsolationR04().sumPUPt;  
	double neutralHadronIso  = mu[j].pfIsolationR04().sumNeutralHadronEt;
	double photonIso  = mu[j].pfIsolationR04().sumPhotonEt;
	double a=0.5;  
	// OPTION 1: DeltaBeta corrections for iosolation
	float RelativeIsolationDBetaCorr = (chargedHadronIso + std::max(photonIso+neutralHadronIso - 0.5*chargedHadronIsoPU,0.))/std::max(a, mu[j].pt());
	patMuonPfIsoDbeta_.push_back(RelativeIsolationDBetaCorr);
	patMuonGlobalType_.push_back(mu[j].isGlobalMuon());
	patMuonTrackerType_.push_back(mu[j].isTrackerMuon());
	patMuonPFType_.push_back(mu[j].isPFMuon());
	patMuonIsoSumPt_.push_back(mu[j].isolationR03().sumPt);
	patMuonIsoRelative_.push_back(mu[j].isolationR03().sumPt/mu[j].pt());
	patMuonIsoCalComb_.push_back(mu[j].isolationR03().emEt + mu[j].isolationR03().hadEt);
	patMuonIsoDY_.push_back((mu[j].isolationR03().sumPt+mu[j].isolationR03().hadEt)/mu[j].pt());
	patMuon_PF_IsoSumChargedHadronPt_.push_back(mu[j].pfIsolationR03().sumChargedHadronPt);
	patMuon_PF_IsoSumNeutralHadronEt_.push_back(mu[j].pfIsolationR03().sumNeutralHadronEt);
	patMuon_PF_IsoDY_.push_back((mu[j].pfIsolationR03().sumChargedHadronPt+mu[j].pfIsolationR03().sumNeutralHadronEt)/mu[j].pt());
	if(mu[j].pt()>22 && abs(mu[j].eta())<2.4 &&idpass>0 &&fabs(RelativeIsolationDBetaCorr)<0.15)MuFill++;
//    }
    }
    }
            //cout<<"eeeeeeeeeeeeeeeeeeeeeee"<<endl;
    //electrons B.B.
  edm::Handle<edm::View<pat::Electron> > electrons;
  iEvent.getByToken(electronToken_,electrons);
  edm::Handle<edm::ValueMap<bool> > ele_vetoid_decisions;
  iEvent.getByToken(eleIdvetoToken_ ,ele_vetoid_decisions);
  edm::Handle<edm::ValueMap<bool> > ele_looseid_decisions;
  iEvent.getByToken(eleIdlooseToken_ ,ele_looseid_decisions);
  edm::Handle<edm::ValueMap<bool> > ele_mediumid_decisions;
  iEvent.getByToken(eleIdmediumToken_ ,ele_mediumid_decisions);
  edm::Handle<edm::ValueMap<bool> > ele_tightid_decisions;
  iEvent.getByToken(eleIdtightToken_ ,ele_tightid_decisions);
  edm::Handle<edm::ValueMap<bool> > ele_HLTPrese;
  iEvent.getByToken(eleHLTPreseToken_ ,ele_HLTPrese);

  int ElecFill=0;
  for (unsigned int j = 0; j < electrons->size(); ++j){
  const auto el = *electrons->ptrAt(j);
  const auto e = electrons->ptrAt(j);

        if(el.pt()<10 || abs(el.eta())>3.0)continue;
  double dEtaIn_;
  double dPhiIn_;
  double hOverE_;
  double sigmaIetaIeta_;
  double full5x5_sigmaIetaIeta_;
  //double relIsoWithDBeta_;
  double ooEmooP_;
  double d0_;
  double dz_;
  int   expectedMissingInnerHits_;
  int   passConversionVeto_;     

     dEtaIn_ =  el.superCluster().isNonnull() && el.superCluster()->seed().isNonnull() ?
           el.deltaEtaSuperClusterTrackAtVtx() - el.superCluster()->eta() + el.superCluster()->seed()->eta() :
           std::numeric_limits<float>::max();
     dPhiIn_ = el.deltaPhiSuperClusterTrackAtVtx();
     hOverE_ = el.hcalOverEcal();
     sigmaIetaIeta_ = el.sigmaIetaIeta();
     full5x5_sigmaIetaIeta_ =  el.full5x5_sigmaIetaIeta();
     if( el.ecalEnergy() == 0 ){
      // printf("Electron energy is zero!\n");
       ooEmooP_ = 1e30;
     }else if( !std::isfinite(el.ecalEnergy())){
      // printf("Electron energy is not finite!\n");
       ooEmooP_ = 1e30;
     }else{
       ooEmooP_ = fabs(1.0/el.ecalEnergy() - el.eSuperClusterOverP()/el.ecalEnergy() );
     }
     d0_ = el.gsfTrack()->dxy(primVtx.position() );
     dz_ = el.gsfTrack()->dz( primVtx.position() );
//if (run==1 && lumi==210023 && event==33645705)cout<<"fabs(d0): "<<d0_<<", fabs(dz):  "<<dz_<<", x: "<<primVtx.x()<<", y: "<<primVtx.y()<<", z: "<<primVtx.z()<<endl;
     expectedMissingInnerHits_ = el.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);//MISSING!!
     passConversionVeto_ = false;
     if( beamSpotHandle.isValid() && conversions_h.isValid()) {
       passConversionVeto_ = !ConversionTools::hasMatchedConversion(el,conversions_h,
								   beamSpotHandle->position());
     }else{
       printf("\n\nERROR!!! conversions not found!!!\n");
     }


     ////cout<<dEtaIn_<<"  "<<dPhiIn_<<"  "<<hOverE_<<"  "<<sigmaIetaIeta_<<"  "<<full5x5_sigmaIetaIeta_<<"  "<<ooEmooP_<<"  "<< d0_<<"  "<< dz_<<"  "<<expectedMissingInnerHits_<<"  "<<passConversionVeto_<<endl;

     patElecdEtaInSeed_.push_back(dEtaIn_);
     patElecdPhiIn_.push_back(dPhiIn_);
     patElechOverE_.push_back(hOverE_);
     patElecsigmaIetaIeta_.push_back(sigmaIetaIeta_);
     patElecfull5x5_sigmaIetaIeta_.push_back(full5x5_sigmaIetaIeta_);
     patElecooEmooP_.push_back(ooEmooP_);
     patElecd0_.push_back(d0_);
     patElecdz_.push_back(dz_);
     patElecexpectedMissingInnerHits_.push_back(expectedMissingInnerHits_);
     patElecpassConversionVeto_.push_back(passConversionVeto_);     

      std::vector<std::pair<std::string,float> > idlist = el.electronIDs();
      if(!elecIdsListed_) {
	std::cout << "Supported electron ids:\n"; 
	for (unsigned k  = 0 ; k < idlist.size(); ++k){
	  std::cout << idlist[k].first << ": " << idlist[k].second  << "\n";
	}
	std::cout << std::flush;
	elecIdsListed_ = true;
      }
           bool isPassEle_vetoId = (*ele_vetoid_decisions)[e];
           bool isPassEle_looseId = (*ele_looseid_decisions)[e];
           bool isPassEle_mediumId = (*ele_mediumid_decisions)[e];
           bool isPassEle_tightId = (*ele_tightid_decisions)[e];
           bool isPassEle_HLTPrese = (*ele_HLTPrese)[e];
           patElecIdveto_.push_back(isPassEle_vetoId);
           patElecIdloose_.push_back(isPassEle_looseId);
           patElecIdmedium_.push_back(isPassEle_mediumId);
           patElecIdtight_.push_back(isPassEle_tightId);
      	   patElec_mva_presel_.push_back(isPassEle_HLTPrese);
      reco::Vertex::Point pos(0,0,0);
      if(vtx_h->size() > 0) pos = vtx_h->at(0).position();
      patElecDz_.push_back(el.gsfTrack()->dz(pos));
      patElecPt_.push_back(el.pt());
      patElecEta_.push_back(el.eta());
      
      patElecScEta_.push_back(el.superCluster()->eta());
      patElecPhi_.push_back(el.phi());	
      patElecEnergy_.push_back(el.energy());
      patElecCharge_.push_back(el.charge());
	float elec_SCEta = fabs(el.superCluster()->eta());
	if (0.0 < elec_SCEta && elec_SCEta <= 1.0)AEff=0.1703;	
	if (1.0 < elec_SCEta && elec_SCEta <= 1.479)AEff=0.1715;	
	if (1.479 < elec_SCEta && elec_SCEta <= 2.0)AEff=0.1213;	
	if (2.0 < elec_SCEta && elec_SCEta <= 2.2)AEff=0.1230;	
	if (2.2 < elec_SCEta && elec_SCEta <= 2.3)AEff=0.1635;	
	if (2.3 < elec_SCEta && elec_SCEta <= 2.4)AEff=0.1937;	
	if (2.4 < elec_SCEta && elec_SCEta <= 5.0)AEff=0.2393;
      rhoPrime = std::max(rhoIso, 0.0);
      const double chIso03_ = el.chargedHadronIso();
      const double nhIso03_ = el.neutralHadronIso();
      const double phIso03_ = el.photonIso();
      const double puChIso03_= el.puChargedHadronIso();
      patElecchIso03_.push_back(chIso03_);
      patElecnhIso03_.push_back(nhIso03_);
      patElecphIso03_.push_back(phIso03_);
      patElecpuChIso03_.push_back(puChIso03_);
      patElecPfIso_.push_back(( chIso03_ + nhIso03_ + phIso03_ ) / el.pt());
      patElecPfIsodb_.push_back(( chIso03_ + max(0.0, nhIso03_ + phIso03_ - 0.5*puChIso03_) )/ el.pt());
      patElecPfIsoRho_.push_back(( chIso03_ + max(0.0, nhIso03_ + phIso03_ - rhoPrime*AEff) )/ el.pt());
      if(el.pt()>20)ElecFill++; 
    }
    double PFjetFill=0;
    double PFjetFill_b=0;
    double chf = 0;
    double nhf = 0;
    double cemf = 0;
    double nemf = 0;
    double muf = 0;
    double cmult = 0;
    double nconst = 0, nNeutralParticles = 0;
    
    //for(edm::View<pat::Jet>::const_iterator jet=jets->begin(); jet!=jets->end(); ++jet){
  edm::Handle<edm::View<pat::Jet> >jets;
  iEvent.getByToken(jetToken_,jets);

    for ( unsigned int i=0; i<jets->size(); ++i ) {
    const auto jet = *jets->ptrAt(i);
    const auto j = jets->ptrAt(i);
      if((channel_!="noselection")&&(jet.pt()<15 || abs(jet.eta())>2.5))continue;
      if((channel_=="noselection")&&(jet.pt()<15 || abs(jet.eta())>5.0))continue;
      chf = jet.chargedHadronEnergyFraction();
      nhf = (jet.neutralHadronEnergy()+jet.HFHadronEnergy())/jet.correctedJet(0).energy();
      cemf = jet.chargedEmEnergyFraction();
      nemf = jet.neutralEmEnergyFraction();
      muf = jet.muonEnergyFraction();
      cmult = jet.chargedMultiplicity();
      nconst = jet.chargedMultiplicity()+jet.neutralMultiplicity();
      nNeutralParticles = jet.neutralMultiplicity(); 
      patJetPfAk04BDiscdeepFudsg_.push_back(jet.bDiscriminator("deepFlavourJetTags:probudsg"));
      patJetPfAk04BDiscdeepFb_.push_back(jet.bDiscriminator("deepFlavourJetTags:probb"));
      patJetPfAk04BDiscdeepFbb_.push_back(jet.bDiscriminator("deepFlavourJetTags:probbb"));
      patJetPfAk04BDiscdeepFc_.push_back(jet.bDiscriminator("deepFlavourJetTags:probc"));
      patJetPfAk04BDiscdeepFcc_.push_back(jet.bDiscriminator("deepFlavourJetTags:probcc"));
      patJetPfAk04BDiscCSVv2_.push_back(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
      patJetPfAk04BDiscpfCMVA_.push_back(jet.bDiscriminator("pfCombinedMVAV2BJetTags"));
      patJetPfAk04BDiscCSVSLV1_.push_back(jet.bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags"));
      
      patJetPfAk04En_.push_back(jet.energy());
      patJetPfAk04Pt_.push_back(jet.pt());
      patJetPfAk04Eta_.push_back(jet.eta());
      patJetPfAk04Phi_.push_back(jet.phi());
      patJetPfAk04Et_.push_back(jet.et());
      patJetPfAk04PartonFlavour_.push_back(jet.partonFlavour());
      patJetPfAk04HadronFlavour_.push_back(jet.hadronFlavour());
      patJetPfAk04RawPt_.push_back(jet.correctedJet(0).pt());
      patJetPfAk04RawEn_.push_back(jet.correctedJet(0).energy());
      patJetPfAk04HadEHF_.push_back(jet.HFHadronEnergy());
      patJetPfAk04EmEHF_.push_back(jet.HFEMEnergy());
      patJetPfAk04chf_.push_back(chf);
      patJetPfAk04nhf_.push_back(nhf);
      patJetPfAk04cemf_.push_back(cemf);
      patJetPfAk04nemf_.push_back(nemf);
      patJetPfAk04muf_.push_back(muf);
      patJetPfAk04cmult_.push_back(cmult);
      patJetPfAk04nconst_.push_back(nconst);


      jecUnc->setJetEta(jet.eta());
      jecUnc->setJetPt(jet.pt());
      double unc = jecUnc->getUncertainty(true);

//      double unc = 1.;
      unc_.push_back(unc);
      double up = (1+unc)*jet.pt();
      double down = (1-unc)*jet.pt();
      patJetPfAk04PtUp_.push_back(up);
      patJetPfAk04PtDn_.push_back(down);
//https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2016
      double tempJetID=0;
      double jeta=jet.eta();
	if((nhf<0.99 && nemf<0.99 && nconst>1) && ((abs(jeta)<=2.4 && chf>0 && cmult>0 && cemf<0.99) || abs(jeta)>2.4) && abs(jeta)<=2.7)tempJetID=1; 
	if((nhf<0.90 && nemf<0.90 && nconst>1) && ((abs(jeta)<=2.4 && chf>0 && cmult>0 && cemf<0.99) || abs(jeta)>2.4) && abs(jeta)<=2.7)tempJetID=2; 
	if((nhf<0.90 && nemf<0.90 && nconst>1&& (muf<0.80)) && ((abs(jeta)<=2.4 && chf>0 && cmult>0 && cemf<0.90) || abs(jeta)>2.4) && abs(jeta)<=2.7)tempJetID=3; 

	if(nhf<0.98 && nemf>0.01 && nNeutralParticles>2 && abs(jeta)>2.7 && abs(jeta)<=3.0  )tempJetID=1;
	if(nhf<0.98 && nemf>0.01 && nNeutralParticles>2 && abs(jeta)>2.7 && abs(jeta)<=3.0  )tempJetID=2;

	if(nemf<0.90 && nNeutralParticles>10 && abs(jeta)>3.0 )tempJetID=1;
	if(nemf<0.90 && nNeutralParticles>10 && abs(jeta)>3.0)tempJetID=2;
      patJetPfAk04LooseId_.push_back(tempJetID);//ala 
      if(jet.pt()>30 &&tempJetID>0)PFjetFill++;
      if(jet.pt()>30 &&tempJetID>0&& jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")>=0.8)PFjetFill_b++;
      for(unsigned int idx =0; idx<jet.numberOfDaughters();idx++){
     // cout<<jet.numberOfDaughters()<<" RECO AHMEEEEET "<<idx<<"  "<<jet.daughter(idx)->pdgId()<<"  "<<endl;
      patJetPfAk04ConstId.push_back(jet.daughter(idx)->pdgId());
      patJetPfAk04ConstPt.push_back(jet.daughter(idx)->pt());
      patJetPfAk04ConstCharge.push_back(jet.daughter(idx)->charge());
      patJetPfAk04ConstEta.push_back(jet.daughter(idx)->eta());
      patJetPfAk04ConstPhi.push_back(jet.daughter(idx)->phi());
      patJetPfAk04ConstE.push_back(jet.daughter(idx)->energy());


    }
      double smear=jet.pt(), smearUp=jet.pt(),smearDn=jet.pt();
      double smearE=jet.energy(), smearUpE=jet.energy(),smearDnE=jet.energy();
  //    smear = getJER(jet.eta(), 0); //JER nominal=0, up=+1, down=-1
  //    smearUp = getJER(jet.eta(), 1); //JER nominal=0, up=+1, down=-1
  //    smearDn = getJER(jet.eta(), -1); //JER nominal=0, up=+1, down=-1
      if(!realdata){
        bool matchGen=false;
	JME::JetResolution resolution = JME::JetResolution::get(iSetup, "AK4PFchs_pt");
	JME::JetResolutionScaleFactor resolution_sf = JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs");
        JME::JetParameters parameters_1;
        parameters_1.setJetPt(jet.pt());
        parameters_1.setJetEta(jet.eta());
        parameters_1.setRho(*rho_);
        double r = resolution.getResolution(parameters_1);
        double sf = resolution_sf.getScaleFactor({{JME::Binning::JetEta, jet.eta()}});

        // Access up and down variation of the scale factor
        double sf_up = resolution_sf.getScaleFactor({{JME::Binning::JetEta, jet.eta()}}, Variation::UP);
        double sf_dn = resolution_sf.getScaleFactor({{JME::Binning::JetEta, jet.eta()}}, Variation::DOWN);

//        cout<<gRandom->Gaus(jet.pt(),sqrt(sf*sf-1)*r)<<endl;
        smear=gRandom->Gaus(jet.pt(),sqrt(sf*sf-1)*r);
	//float s = gRandom->Gaus(0,1);
	//smear=s*(sqrt(sf*sf-1)*r)+jet.pt();
        smearUp=gRandom->Gaus(jet.pt(),sqrt(sf_up*sf_up-1)*r);
        smearDn=gRandom->Gaus(jet.pt(),sqrt(sf_dn*sf_dn-1)*r);
	if (jet.genJet()){
    TLorentzVector jet_vv;
    TLorentzVector gjet_vv;
    jet_vv.SetPtEtaPhiE(jet.pt(),jet.eta(),jet.phi(),jet.energy());
    gjet_vv.SetPtEtaPhiE(jet.genJet()->pt(),jet.genJet()->eta(),jet.genJet()->phi(),jet.genJet()->energy());
    double DR_gj_j=jet_vv.DeltaR(gjet_vv);
    double DPt_gj_j=fabs(jet.pt()-jet.genJet()->pt());
    if( DR_gj_j<0.2 &&DPt_gj_j<3*r*jet.pt() ){
//    if( DR_gj_j<0.2 ){
	  matchGen=true;
	  MGjPt.push_back(jet.genJet()->pt());
	  MGjeta.push_back(jet.genJet()->eta());
	  MGjphi.push_back(jet.genJet()->phi());
	  MGjE.push_back(jet.genJet()->energy());
          smear=std::max(0.0,jet.genJet()->pt() +sf *( jet.pt()-jet.genJet()->pt() ) );
//        smearUp=gRandom->Gaus(jet.pt(),sqrt(sf_up*sf_up-1)*r);
          smearUp=std::max(0.0,jet.genJet()->pt() +sf_up *( jet.pt()-jet.genJet()->pt() ) );
          smearDn=std::max(0.0,jet.genJet()->pt() +sf_dn *( jet.pt()-jet.genJet()->pt() ) );
         // cout<<"Burdayım ulan  "<<smear<<"  "<<smearUp<<"  "<<smearDn<<endl;
	}}
        //cout<<smear<<"  "<<smearUp<<"  "<<smearDn<<endl;
	matchGjet.push_back(matchGen);
        smearE=jet.energy()*smear/jet.pt();
        smearUpE=jet.energy()*smearUp/jet.pt();
        smearDnE=jet.energy()*smearDn/jet.pt();
      }
      patJetPfAk04PtJERSmear.push_back(smear);
      patJetPfAk04PtJERSmearUp.push_back(smearUp);
      patJetPfAk04PtJERSmearDn.push_back(smearDn);
      patJetPfAk04EnJERSmear.push_back(smearE);
      patJetPfAk04EnJERSmearUp.push_back(smearUpE);
      patJetPfAk04EnJERSmearDn.push_back(smearDnE);
    }
    
    //end jets
                //cout<<"gggggggggggggggggggggg"<<endl;
       accept=false;
     wtot_write+= w;
     //cout<<wtot_write<<endl;
if(
  (channel_=="dimu" && ((MuFill>=2&&PFjetFill>=2&&PFjetFill_b>=2)  ||(gendimu &&ngjets>=2))  )
||(channel_=="smu" && ((MuFill==1&&PFjetFill>=4&&PFjetFill_b>=2 )  ||(gensinglemu&&ngjets>=4))   )
||(channel_=="dielec" && ((ElecFill>=2&&PFjetFill>=2&&PFjetFill_b>=2)  ||(gendielec&&ngjets>=2))  )
||(channel_=="emu" && ((ElecFill>=1&&MuFill>=1&&PFjetFill>=2&&PFjetFill_b>=2)  ||(genemu&&ngjets>=2))  )
||(channel_=="selec" && ( (ElecFill>=1&&PFjetFill>=4&&PFjetFill_b>=2)  ||(gensingleelec&&ngjets>=4) )   )  
||(channel_=="noselection")
 ){
     // cout<<"accept "<<accept<<"  "<<wtot_write<<endl;
//    cout<<(MuFill==1&&PFjetFill>=4&&PFjetFill_b>=2 )<<"  "<<(gensinglemu &&ngjets>=4)<<"  "<<((MuFill==1&&PFjetFill>=4&&PFjetFill_b>=2 )  ||(gensinglemu&&ngjets>=4))<<endl;
      myTree->Fill();
      wtot_write=0;
      accept=true;
   //        cout<<"accept after "<<accept<<"  "<<wtot_write<<endl;
   }               //cout<<"hhhhhhhhhhhhhhhhhhhhh"<<endl;
}

void
Tupel::beginRun(edm::Run const& iRun, edm::EventSetup const&){
}
void 
Tupel::beginJob()
{
    // register to the TFileService
    edm::Service<TFileService> fs;
    TFileDirectory TestDir = fs->mkdir("test");
    myTree = new TTree("MuonTree","MuonTree");
    myTree->Branch("wtot_write",&wtot_write);
    myTree->Branch("accept",&accept);

    if(keepparticlecoll_){
      myTree->Branch("Packed01Pt",&Packed01Pt);
      myTree->Branch("Packed01Eta",&Packed01Eta);
      myTree->Branch("Packed01Phi",&Packed01Phi);
      myTree->Branch("Packed01E",&Packed01E);
      myTree->Branch("Packed01M",&Packed01M);
      myTree->Branch("Packed01Id",&Packed01Id);
      myTree->Branch("Packed01Status",&Packed01Status);
      myTree->Branch("Packed01MomId",&Packed01MomId);
      myTree->Branch("Packed01Charge",&Packed01Charge);
      myTree->Branch("Packed01IsPrompt",&Packed01IsPrompt);
      myTree->Branch("Packed01IsTauProd",&Packed01IsTauProd);
    myTree->Branch("patPfCandPt",&patPfCandPt);
    myTree->Branch("patPfCandEta",&patPfCandEta);
    myTree->Branch("patPfCandPhi",&patPfCandPhi);
    myTree->Branch("patPfCandE",&patPfCandE);
    myTree->Branch("patPfCandM",&patPfCandM);
    myTree->Branch("patPfCandPx",&patPfCandPx);
    myTree->Branch("patPfCandPhiAtVtx",&patPfCandPhiAtVtx);
    myTree->Branch("patPfCandLostInnerHits",&patPfCandLostInnerHits);
    myTree->Branch("patPfCandTrackHighPurity",&patPfCandTrackHighPurity);
    myTree->Branch("patPfCandPuppiWeight",&patPfCandPuppiWeight);
    myTree->Branch("patPfCandPuppiWeightNolep",&patPfCandPuppiWeightNolep);
    myTree->Branch("patPfCandCharge",&patPfCandCharge);
    myTree->Branch("patPfCandDxy",&patPfCandDxy);
    myTree->Branch("patPfCandDxyerr",&patPfCandDxyerr);
    myTree->Branch("patPfCandDz",&patPfCandDz);
    myTree->Branch("patPfCandDzAssociatedPV",&patPfCandDzAssociatedPV);
    myTree->Branch("patPfCandDzerr",&patPfCandDzerr);
    myTree->Branch("patPfCandFromPv",&patPfCandFromPv);
    myTree->Branch("patPfCandVertexRef",&patPfCandVertexRef);
    myTree->Branch("patPfpvAssociationQuality",&patPfpvAssociationQuality);
    myTree->Branch("Dr01LepPt",&Dr01LepPt);
    myTree->Branch("Dr01LepEta",&Dr01LepEta);
    myTree->Branch("Dr01LepPhi",&Dr01LepPhi);
    myTree->Branch("Dr01LepE",&Dr01LepE);
    myTree->Branch("Dr01LepM",&Dr01LepM);
    myTree->Branch("Dr01LepId",&Dr01LepId);
    myTree->Branch("Dr01LepStatus",&Dr01LepStatus);
    myTree->Branch("Dr01LepMomId",&Dr01LepMomId);
    myTree->Branch("Dr01LepIsPrompt",&Dr01LepIsPrompt);
    myTree->Branch("Dr01LepIsTauProd",&Dr01LepIsTauProd);

    myTree->Branch("Bare01LepPt",&Bare01LepPt);
    myTree->Branch("Bare01LepEta",&Bare01LepEta);
    myTree->Branch("Bare01LepPhi",&Bare01LepPhi);
    myTree->Branch("Bare01LepE",&Bare01LepE);
    myTree->Branch("Bare01LepM",&Bare01LepM);
    myTree->Branch("Bare01LepId",&Bare01LepId);
    myTree->Branch("Bare01LepStatus",&Bare01LepStatus);
    myTree->Branch("Bare01LepMomId",&Bare01LepMomId);      

    }

    myTree->Branch("Uncorec_METPt",&Uncorec_METPt);
    myTree->Branch("Uncorec_METPhi",&Uncorec_METPhi);
    myTree->Branch("METPt",&METPt);
    myTree->Branch("METPx",&METPx);
    myTree->Branch("METPy",&METPy);
    myTree->Branch("METPz",&METPz);
    myTree->Branch("METE",&METE);
    myTree->Branch("METsigx2",&METsigx2);
    myTree->Branch("METsigxy",&METsigxy);
    myTree->Branch("METsigy2",&METsigy2);
    myTree->Branch("METsig",&METsig);
    myTree->Branch("METUncPtUp",&METUncPtUp);
    myTree->Branch("METUncPtDn",&METUncPtDn);
    myTree->Branch("METUncPhiUp",&METUncPhiUp);
    myTree->Branch("METUncPhiDn",&METUncPhiDn);
    myTree->Branch("event",&event);
    myTree->Branch("realdata",&realdata);
    myTree->Branch("run",&run);
    myTree->Branch("lumi",&lumi);
    myTree->Branch("bxnumber",&bxnumber);
    myTree->Branch("EvtInfo_NumVtx",&EvtInfo_NumVtx);
    myTree->Branch("first_PV",&first_PV);
    myTree->Branch("PU_npT",&PU_npT);
    myTree->Branch("PU_npIT",&PU_npIT);

    myTree->Branch("St03Pt",&St03Pt);
    myTree->Branch("St03Eta",&St03Eta);
    myTree->Branch("St03Phi",&St03Phi);
    myTree->Branch("St03E",&St03E);
    myTree->Branch("St03M",&St03M);
    myTree->Branch("St03Id",&St03Id);
    myTree->Branch("St03Status",&St03Status);
    myTree->Branch("St03MotherId",&St03MotherId);
    myTree->Branch("St03NumberMom",&St03NumberMom);    

    myTree->Branch("matchGjet",&matchGjet);
    myTree->Branch("MGjPt",&MGjPt);
    myTree->Branch("MGjeta",&MGjeta);
    myTree->Branch("MGjphi",&MGjphi);
    myTree->Branch("MGjE",&MGjE); 
    
    //HLT
     myTree->Branch("HLT_Ele32_eta2p1_WPTight_Gsf",&HLT_Ele32_eta2p1_WPTight_Gsf);
     myTree->Branch("HLT_IsoMu24",&HLT_IsoMu24); 
     myTree->Branch("HLT_IsoTkMu24",&HLT_IsoTkMu24); 
     myTree->Branch("HLT_IsoMu22_v",&HLT_IsoMu22_v); 
     myTree->Branch("HLT_IsoTkMu22_v",&HLT_IsoTkMu22_v); 

     myTree->Branch("Flag_HBHENoiseFilter",&Flag_HBHENoiseFilter); 
     myTree->Branch("Flag_HBHENoiseIsoFilter",&Flag_HBHENoiseIsoFilter); 
     myTree->Branch("Flag_globalTightHalo2016Filter",&Flag_globalTightHalo2016Filter); 
     myTree->Branch("Flag_EcalDeadCellTriggerPrimitiveFilter",&Flag_EcalDeadCellTriggerPrimitiveFilter); 
     myTree->Branch("Flag_goodVertices",&Flag_goodVertices); 
     myTree->Branch("Flag_eeBadScFilter",&Flag_eeBadScFilter); 
     myTree->Branch("Flag_chargedHadronTrackResolutionFilter",&Flag_chargedHadronTrackResolutionFilter ); 
     myTree->Branch("Flag_muonBadTrackFilter",&Flag_muonBadTrackFilter); 

    
    //Muons
    myTree->Branch("patMuonPt_",&patMuonPt_);
    myTree->Branch("patMuonEta_",&patMuonEta_);
    myTree->Branch("patMuonPhi_",&patMuonPhi_);
    myTree->Branch("patMuonVtxZ_",&patMuonVtxZ_);
    myTree->Branch("patMuonEn_",&patMuonEn_);
    myTree->Branch("patMuonCharge_",&patMuonCharge_);
    myTree->Branch("patMuonDxy_",&patMuonDxy_);
    myTree->Branch("patMuonCombId_",&patMuonCombId_);
    myTree->Branch("patMuonLooseId_",&patMuonLooseId_);
    myTree->Branch("patMuonMediumId_",&patMuonMediumId_);
    myTree->Branch("patMuonTightId_",&patMuonTightId_);
    myTree->Branch("patMuonDetIsoRho_",&patMuonDetIsoRho_);
    myTree->Branch("patMuonPfIsoDbeta_",&patMuonPfIsoDbeta_);
    myTree->Branch("patMuonM_",&patMuonM_);
    myTree->Branch("patMuonPx_",&patMuonPx_);
    myTree->Branch("patMuonPy_",&patMuonPy_);
    myTree->Branch("patMuonPz_",&patMuonPz_);
    myTree->Branch("patMuonGlobalType_",&patMuonGlobalType_);
    myTree->Branch("patMuonTrackerType_",&patMuonTrackerType_);
    myTree->Branch("patMuonPFType_",&patMuonPFType_);
    myTree->Branch("patMuonIsoSumPt_",&patMuonIsoSumPt_);
    myTree->Branch("patMuonIsoRelative_",&patMuonIsoRelative_);
    myTree->Branch("patMuonIsoCalComb_",&patMuonIsoCalComb_);
    myTree->Branch("patMuonIsoDY_",&patMuonIsoDY_);
    myTree->Branch("patMuonChi2Ndoff_",&patMuonChi2Ndoff_);
    myTree->Branch("patMuonNhits_",&patMuonNhits_);
    myTree->Branch("patMuonNMatches_",&patMuonNMatches_);
    myTree->Branch("patMuonDz_",&patMuonDz_);
    myTree->Branch("patMuonPhits_",&patMuonPhits_);
    myTree->Branch("patMuonTkLayers_",&patMuonTkLayers_);
    myTree->Branch("patMuon_PF_IsoSumChargedHadronPt_",&patMuon_PF_IsoSumChargedHadronPt_);
    myTree->Branch("patMuon_PF_IsoSumNeutralHadronEt_",&patMuon_PF_IsoSumNeutralHadronEt_);
    myTree->Branch("patMuon_PF_IsoDY_",&patMuon_PF_IsoDY_);

    myTree->Branch("patElecIdveto_",&patElecIdveto_);
    myTree->Branch("patElecIdloose_",&patElecIdloose_);
    myTree->Branch("patElecIdmedium_",&patElecIdmedium_);
    myTree->Branch("patElecIdtight_",&patElecIdtight_);
    myTree->Branch("patElecdEtaInSeed_",&patElecdEtaInSeed_);
    myTree->Branch("patElecdPhiIn_",&patElecdPhiIn_);
    myTree->Branch("patElechOverE_",&patElechOverE_);
    myTree->Branch("patElecsigmaIetaIeta_",&patElecsigmaIetaIeta_);
    myTree->Branch("patElecfull5x5_sigmaIetaIeta_",&patElecfull5x5_sigmaIetaIeta_);
    myTree->Branch("patElecooEmooP_",&patElecooEmooP_);
    myTree->Branch("patElecd0_",&patElecd0_);
    myTree->Branch("patElecdz_",&patElecdz_);
    myTree->Branch("patElecexpectedMissingInnerHits_",&patElecexpectedMissingInnerHits_);
    myTree->Branch("patElecpassConversionVeto_",&patElecpassConversionVeto_);     
    myTree->Branch("patElecDz_",&patElecDz_);
    myTree->Branch("patElecPt_",&patElecPt_);
    myTree->Branch("patElecEta_",&patElecEta_);
    myTree->Branch("patElecScEta_",&patElecScEta_);
    myTree->Branch("patElecPhi_",&patElecPhi_);
    myTree->Branch("patElecEnergy_",&patElecEnergy_);
    myTree->Branch("patElecCharge_",&patElecCharge_);

    myTree->Branch("patElecchIso03_",&patElecchIso03_);
    myTree->Branch("patElecnhIso03_",&patElecnhIso03_);
    myTree->Branch("patElecphIso03_",&patElecphIso03_);
    myTree->Branch("patElecpuChIso03_",&patElecpuChIso03_);
    myTree->Branch("patElecPfIso_",&patElecPfIso_);
    myTree->Branch("patElecPfIsodb_",&patElecPfIsodb_);
    myTree->Branch("patElecPfIsoRho_",&patElecPfIsoRho_);
    myTree->Branch("rhoPrime",&rhoPrime);
    myTree->Branch("charged_",&charged_);
    myTree->Branch("AEff",&AEff);
    myTree->Branch("patElec_mva_presel_",&patElec_mva_presel_);
    
    //PFJet
    myTree->Branch("patJetPfAk04PtJERSmear",&patJetPfAk04PtJERSmear);
    myTree->Branch("patJetPfAk04PtJERSmearUp",&patJetPfAk04PtJERSmearUp);
    myTree->Branch("patJetPfAk04PtJERSmearDn",&patJetPfAk04PtJERSmearDn);
    myTree->Branch("patJetPfAk04EnJERSmear",&patJetPfAk04EnJERSmear);
    myTree->Branch("patJetPfAk04EnJERSmearUp",&patJetPfAk04EnJERSmearUp);
    myTree->Branch("patJetPfAk04EnJERSmearDn",&patJetPfAk04EnJERSmearDn);
    myTree->Branch("patJetPfAk04En_",&patJetPfAk04En_);
    myTree->Branch("patJetPfAk04Pt_",&patJetPfAk04Pt_);
    myTree->Branch("patJetPfAk04Eta_",&patJetPfAk04Eta_);
    myTree->Branch("patJetPfAk04Phi_",&patJetPfAk04Phi_);
    myTree->Branch("patJetPfAk04LooseId_",&patJetPfAk04LooseId_);
    myTree->Branch("patJetPfAk04Et_",&patJetPfAk04Et_);
    myTree->Branch("patJetPfAk04PartonFlavour_",&patJetPfAk04PartonFlavour_);
    myTree->Branch("patJetPfAk04HadronFlavour_",&patJetPfAk04HadronFlavour_);
    myTree->Branch("patJetPfAk04RawPt_",&patJetPfAk04RawPt_);
    myTree->Branch("patJetPfAk04RawEn_",&patJetPfAk04RawEn_);
    myTree->Branch("patJetPfAk04HadEHF_",&patJetPfAk04HadEHF_);
    myTree->Branch("patJetPfAk04EmEHF_",&patJetPfAk04EmEHF_);
    myTree->Branch("patJetPfAk04chf_",&patJetPfAk04chf_);
    myTree->Branch("patJetPfAk04nhf_",&patJetPfAk04nhf_);
    myTree->Branch("patJetPfAk04cemf_",&patJetPfAk04cemf_);
    myTree->Branch("patJetPfAk04nemf_",&patJetPfAk04nemf_);
    myTree->Branch("patJetPfAk04muf_",&patJetPfAk04muf_);
    myTree->Branch("patJetPfAk04cmult_",&patJetPfAk04cmult_);
    myTree->Branch("patJetPfAk04nconst_",&patJetPfAk04nconst_);
    myTree->Branch("patJetPfAk04BDiscdeepFudsg_",&patJetPfAk04BDiscdeepFudsg_);
    myTree->Branch("patJetPfAk04BDiscdeepFb_",&patJetPfAk04BDiscdeepFb_);
    myTree->Branch("patJetPfAk04BDiscdeepFbb_",&patJetPfAk04BDiscdeepFbb_);
    myTree->Branch("patJetPfAk04BDiscdeepFc_",&patJetPfAk04BDiscdeepFc_);
    myTree->Branch("patJetPfAk04BDiscdeepFcc_",&patJetPfAk04BDiscdeepFcc_);
    myTree->Branch("patJetPfAk04BDiscCSVv2_",&patJetPfAk04BDiscCSVv2_);
    myTree->Branch("patJetPfAk04BDiscpfCMVA_",&patJetPfAk04BDiscpfCMVA_);
    myTree->Branch("patJetPfAk04BDiscCSVSLV1_",&patJetPfAk04BDiscCSVSLV1_);
    myTree->Branch("unc_",&unc_);
    myTree->Branch("patJetPfAk04PtUp_",&patJetPfAk04PtUp_);
    myTree->Branch("patJetPfAk04PtDn_",&patJetPfAk04PtDn_); 
    myTree->Branch("patJetPfAk04ConstId",&patJetPfAk04ConstId);
    myTree->Branch("patJetPfAk04ConstPt",&patJetPfAk04ConstPt);
    myTree->Branch("patJetPfAk04ConstCharge",&patJetPfAk04ConstCharge);
    myTree->Branch("patJetPfAk04ConstEta",&patJetPfAk04ConstEta);
    myTree->Branch("patJetPfAk04ConstPhi",&patJetPfAk04ConstPhi);
    myTree->Branch("patJetPfAk04ConstE",&patJetPfAk04ConstE);

    //CaloJets
    
    myTree->Branch("id1_pdfInfo_",&id1_pdfInfo_);
    myTree->Branch("id2_pdfInfo_",&id2_pdfInfo_);
    myTree->Branch("x1_pdfInfo_",&x1_pdfInfo_);
    myTree->Branch("x2_pdfInfo_",&x2_pdfInfo_);
    myTree->Branch("scalePDF_pdfInfo_",&scalePDF_pdfInfo_);
    myTree->Branch("ptHat_",&ptHat_);
    myTree->Branch("mcWeight_",&mcWeight_);
    myTree->Branch("mcWeights_",&mcWeights_);
    myTree->Branch("nup",&nup);   
}
/*
double Tupel::getJER(double jetEta, int sysType){
  
  double jerSF = 1.0;

  if (! (sysType==0 || sysType==-1 || sysType==1)){
    cout<< "ERROR: Can't get JER! use type=0 (nom), -1 (down), +1 (up)"<<endl;
    return jerSF;
  }
    // Values from https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
  double etamin[] = {0.0,0.5,1.1,1.7,2.3,2.8,3.2};
  double etamax[] = {0.5,1.1,1.7,2.3,2.8,3.2,5.0};
    
  double scale_nom[]  = {1.079,1.099,1.121,1.208,1.254,1.395,1.056};
  double scale_dn[]   = {1.053,1.071,1.092,1.162,1.192,1.332,0.865};
  double scale_up[]   = {1.105,1.127,1.150,1.254,1.316,1.458,1.247};

//    for iSF in range(0,len(scale_nom)) :
  for(int iSF=0;iSF<7;iSF++){//hardcoded size of array- to be checked if change

    if (abs(jetEta) >= etamin[iSF] && abs(jetEta) < etamax[iSF]) {
      //cout<<"I am here"<<endl;
      if (sysType < 0) jerSF = scale_dn[iSF];
      else if (sysType > 0)jerSF = scale_up[iSF];
      else jerSF = scale_nom[iSF];
      break;
    }
  }
  return jerSF;
}
*/
void 
Tupel::endJob() 
{
//  delete jecUnc;
if(!accept)myTree->Fill();
  myTree->Print();
//  if(!accept)cout<<"ended job"<<endl;
}

DEFINE_FWK_MODULE(Tupel);
