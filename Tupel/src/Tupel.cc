/* -*- c-basic-offset: 2; -*-

Code by: Bugra Bilin, Kittikul Kovitanggoon, Tomislav Seva, Efe Yazgan,
         Philippe Gras...

*/

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <TLorentzVector.h>
#include <stdlib.h>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "FWCore/Utilities/interface/TimeOfDay.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Version/interface/GetReleaseVersion.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "EgammaAnalysis/ElectronTools/interface/EGammaCutBasedEleId.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
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
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "TreeHelper.h"

const double pi = 4*atan(1.);

#define QUOTE2(a) #a
#define QUOTE(a) QUOTE2(a)
const static char* checksum = QUOTE(MYFILE_CHECKSUM);

/** 
 * Define a bit of a bit field.
 * 1. Document the bit in the ROOT tree
 * 2. Set the variable with the bit mask (integer with the relevant bit set).
 * The second version DEF_BIT2 omits this setting. It can be useful when we
 * want to avoid defining the variable.
 * 3. Set a map to match bit name to bit mask
 * A field named after label prefixed with a k and with an undercore
 * appended to it must be defined in the class members. This macro will set it.
 * @param bitField: name (verbatim string without quotes) of the event tree branch the bit belongs to
 * @param bitNum: bit position, LSB is 0, MSB is 31
 * @param label: used to build the class field names (k<label>_ for the mask
 * and <label>Map_ for the name->mask map). The label is stored as
 * description in the ROOT file. Use a verbatim string without quotes.
 * 
 *  For long description including spaces please use the DEF_BIT_L version
 */
#define DEF_BIT(bitField, bitNum, label) \
  k ## label ## _ = 1 <<bitNum; \
  bitField ## Map ## _[#label] = 1LL <<bitNum;		\
  treeHelper_->defineBit(#bitField, bitNum, #label);

#define DEF_BIT2(bitField, bitNum, label) \
  bitField ## Map ## _[#label] = 1LL <<bitNum; \
  treeHelper_->defineBit(#bitField, bitNum, #label);

/**
 * See DEF_BIT. Version for long description which can not be used
 * as variable name. The argument desc must be a c string including
 * the quotes or a c-string (char*) type variable.
 */
#define DEF_BIT_L(bitField, bitNum, label, desc) \
  k ## label ## _ = 1 <<bitNum; \
  bitField ## Map ## _[#label] = 1 <<bitNum; \
  treeHelper_->defineBit(#bitField, bitNum, desc);

#define DEF_BIT2_L(bitField, bitNum, label, desc) \
  bitField ## Map ## _[#label] = 1 <<bitNum; \
  treeHelper_->defineBit(#bitField, bitNum, desc);


//#name -> "name", name ## _ -> name_
#define ADD_BRANCH_D(name, desc) treeHelper_->addBranch(#name, name ## _, desc)
#define ADD_BRANCH(name) treeHelper_->addBranch(#name, name ## _)
#define ADD_MOMENTUM_BRANCH_D(name, desc) treeHelper_->addMomentumBranch(#name, name ## Pt_, name ## Eta_, name ## Phi_, name ## E_, desc)
#define ADD_MOMENTUM_BRANCH(name) treeHelper_->addMomentumBranch(#name, name ## Pt_, name ## Eta_, name ## Phi_, name ## E_)


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
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  /// everything that needs to be done after the event loop

  virtual void endJob();

  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&);

  void defineBitFields();
  
  //help function to compute pseudo rapidity of a TLorentz without
  //ROOT exception in case of infinite pseudo rapidity
  double eta(TLorentzVector v){
    if(fabs(v.CosTheta()) < 1) return -0.5 * log((1.0-v.CosTheta())/(1.0+v.CosTheta()));
    else if(v.Pz() > 0) return 10e10;
    else return -10e10;
  }

  //Write Job information tree in the output. The tree
  //contains one event, with general information
  void writeHeader();

  //Retrieves event objets
  void readEvent(const edm::Event& iEvent);

  //Processes MET
  void processMET(const edm::Event& iEvent);

  //Processes vertices
  void processVtx();
		  
  //Processes PU
  void processPu(const edm::Event& iEvent);

  //Processes GenParticles
  void processGenParticles(const edm::Event& iEvent);

  void processGenJets(const edm::Event& iEvent);

  void processPdfInfo(const edm::Event& iEvent);
  
  void processTrigger(const edm::Event& iEvent);

  void processMuons();

  void processElectrons();

  void processJets();
  
  void processPhotons();

  /** Check if the argument is one of the photon
   * HLT trigger considered for TrigHltPhot and if it is set
   * the corresponding bit.
   */
  void fillTrig(const std::string& trigname);
  
  // input tags
  //edm::InputTag trigger_;
  //edm::InputTag triggerEvent_;
  //edm::InputTag triggerSummaryLabel_;
  std::string elecMatch_;
  std::string muonMatch_;
  std::string muonMatch2_;
  edm::InputTag photonSrc_;
  edm::InputTag elecSrc_;
  edm::InputTag muonSrc_;
  edm::InputTag tauSrc_;
  edm::InputTag jetSrc_;
  edm::InputTag gjetSrc_;
  edm::InputTag metSrc_;
  edm::InputTag mSrcRho_;
  //  edm::InputTag CaloJet_;
  edm::InputTag lheSource_;
  edm::InputTag genParticleSrc_;
  std::vector<edm::InputTag> metSources;

  bool photonIdsListed_;
  bool elecIdsListed_;
  bool hltListed_;

  //edm::EDGetTokenT<edm::ValueMap<float> > full5x5SigmaIEtaIEtaMapToken_;

  // ----------member data ---------------------------
  TTree *myTree;
  std::auto_ptr<TreeHelper> treeHelper_;
  
  //Event
  std::auto_ptr<int>      EvtIsRealData_;
  std::auto_ptr<unsigned> EvtNum_;
  std::auto_ptr<unsigned> EvtRunNum_;
  std::auto_ptr<int> 	  EvtLumiNum_;
  std::auto_ptr<int> 	  EvtBxNum_;
  std::auto_ptr<int> 	  EvtVtxCnt_;
  std::auto_ptr<int> 	  EvtPuCnt_;
  std::auto_ptr<int> 	  EvtPuCntTruth_;
  std::auto_ptr<std::vector<double> > EvtWeights_;
  std::auto_ptr<float>    EvtFastJetRho_;
  
  //Trigger
  std::auto_ptr<unsigned>          TrigHlt_;
  std::map<std::string, unsigned>  TrigHltMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltPhot_;
  std::map<std::string, ULong64_t> TrigHltPhotMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltMu_;
  std::map<std::string, ULong64_t> TrigHltMuMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltDiMu_;
  std::map<std::string, ULong64_t> TrigHltDiMuMap_; //bit assignment
  
  //Missing energy
  std::auto_ptr<std::vector<float> > METPt_;
  std::auto_ptr<std::vector<float> > METPx_;
  std::auto_ptr<std::vector<float> > METPy_;
  std::auto_ptr<std::vector<float> > METPz_;
  std::auto_ptr<std::vector<float> > METE_;
  std::auto_ptr<std::vector<float> > METsigx2_;
  std::auto_ptr<std::vector<float> > METsigxy_;
  std::auto_ptr<std::vector<float> > METsigy2_;   
  std::auto_ptr<std::vector<float> > METsig_;

  //Generator level leptons, dressed
  std::auto_ptr<std::vector<float> > 	GLepDr01Pt_;
  std::auto_ptr<std::vector<float> > 	GLepDr01Eta_;
  std::auto_ptr<std::vector<float> > 	GLepDr01Phi_;
  std::auto_ptr<std::vector<float> > 	GLepDr01E_;
  std::auto_ptr<std::vector<unsigned> > GLepDr01Id_;
  std::auto_ptr<std::vector<int> >      GLepDr01St_;
  std::auto_ptr<std::vector<int> >      GLepDr01MomId_;

  //Generator level leptons, not-dressed  
  std::auto_ptr<std::vector<float> > 	GLepBarePt_;
  std::auto_ptr<std::vector<float> > 	GLepBareEta_;
  std::auto_ptr<std::vector<float> > 	GLepBarePhi_;
  std::auto_ptr<std::vector<float> > 	GLepBareE_;
  std::auto_ptr<std::vector<unsigned> > GLepBareId_;
  std::auto_ptr<std::vector<int> > 	GLepBareSt_;
  std::auto_ptr<std::vector<int> > 	GLepBareMomId_;

  //Generator level leptons, status 3
  std::auto_ptr<std::vector<float> > GLepSt3Pt_;
  std::auto_ptr<std::vector<float> > GLepSt3Eta_;
  std::auto_ptr<std::vector<float> > GLepSt3Phi_;
  std::auto_ptr<std::vector<float> > GLepSt3E_;
  std::auto_ptr<std::vector<float> > GLepSt3M_;
  std::auto_ptr<std::vector<int> >   GLepSt3Id_;
  std::auto_ptr<std::vector<int> >   GLepSt3St_;
  std::auto_ptr<std::vector<int> >   GLepSt3Mother0Id_;
  std::auto_ptr<std::vector<int> >   GLepSt3MotherCnt_;

  //Photons in the vicinity of the leptons
  std::auto_ptr<std::vector<float> > GLepClosePhotPt_;
  std::auto_ptr<std::vector<float> > GLepClosePhotEta_;
  std::auto_ptr<std::vector<float> > GLepClosePhotPhi_;
  std::auto_ptr<std::vector<float> > GLepClosePhotE_;
  std::auto_ptr<std::vector<float> > GLepClosePhotM_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotId_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotMother0Id_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotMotherCnt_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotSt_;

  //Gen Jets
  std::auto_ptr<std::vector<float> > GJetAk04Pt_;
  std::auto_ptr<std::vector<float> > GJetAk04Eta_;
  std::auto_ptr<std::vector<float> > GJetAk04Phi_;
  std::auto_ptr<std::vector<float> > GJetAk04E_;
  std::auto_ptr<std::vector<float> > GJetAk04Id_;
  std::auto_ptr<std::vector<float> > GJetAk04PuId_;
  std::auto_ptr<std::vector<float> > GJetAk04ChFrac_;
  std::auto_ptr<std::vector<int> >   GJetAk04ConstCnt_;
  std::auto_ptr<std::vector<int> >   GJetAk04ConstId_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstPt_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstEta_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstPhi_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstE_;

  //Exta generator information
  std::auto_ptr<std::vector<int> >   GPdfId1_;
  std::auto_ptr<std::vector<int> >   GPdfId2_;
  std::auto_ptr<std::vector<float> > GPdfx1_;
  std::auto_ptr<std::vector<float> > GPdfx2_;
  std::auto_ptr<std::vector<float> > GPdfScale_;
  std::auto_ptr<float>               GBinningValue_;
  std::auto_ptr<int>                 GNup_;


  ///Muons
  std::auto_ptr<std::vector<float> > 	MuPt_;
  std::auto_ptr<std::vector<float> > 	MuEta_;
  std::auto_ptr<std::vector<float> > 	MuPhi_;
  std::auto_ptr<std::vector<float> > 	MuE_;
  std::auto_ptr<std::vector<unsigned> > MuId_;
  std::map<std::string, unsigned>       MuIdMap_; //bit assignment
  std::auto_ptr<std::vector<unsigned> > MuIdTight_;
  std::map<std::string, unsigned>    	MuIdTightMap_; //bit assignment
  std::auto_ptr<std::vector<float> > 	MuCh_;
  std::auto_ptr<std::vector<float> > 	MuVtxZ_;
  std::auto_ptr<std::vector<float> > 	MuDxy_;
  std::auto_ptr<std::vector<float> > 	MuIsoRho_;
  std::auto_ptr<std::vector<float> > 	MuPfIso_;
  std::auto_ptr<std::vector<float> > 	MuType_;
  std::map<std::string, unsigned>    	MuTypeMap_; //bit assignment
  std::auto_ptr<std::vector<float> > 	MuIsoTkIsoAbs_;
  std::auto_ptr<std::vector<float> > 	MuIsoTkIsoRel_;
  std::auto_ptr<std::vector<float> > 	MuIsoCalAbs_;
  std::auto_ptr<std::vector<float> > 	MuIsoCombRel_;
  std::auto_ptr<std::vector<float> > 	MuTkNormChi2_;
  std::auto_ptr<std::vector<int> > 	MuTkHitCnt_;
  std::auto_ptr<std::vector<int> > 	MuMatchedStationCnt_;
  std::auto_ptr<std::vector<float> > 	MuDz_;
  std::auto_ptr<std::vector<int> > 	MuPixelHitCnt_;
  std::auto_ptr<std::vector<int> > 	MuTkLayerCnt_;
  std::auto_ptr<std::vector<float> > 	MuPfIsoChHad_;
  std::auto_ptr<std::vector<float> > 	MuPfIsoNeutralHad_;
  std::auto_ptr<std::vector<float> > 	MuPfIsoRawRel_;
  std::auto_ptr<std::vector<unsigned> > MuHltMatch_;

  //Electrons
  std::auto_ptr<std::vector<float> > 	ElPt_;
  std::auto_ptr<std::vector<float> > 	ElEta_;
  std::auto_ptr<std::vector<float> > 	ElEtaSc_;
  std::auto_ptr<std::vector<float> > 	ElPhi_;
  std::auto_ptr<std::vector<float> > 	ElE_;
  std::auto_ptr<std::vector<float> > 	ElCh_;  
  std::auto_ptr<std::vector<unsigned> > ElId_;
  std::map<std::string, unsigned>    	ElIdMap_; //bit assignment
  std::auto_ptr<std::vector<float> > 	ElMvaTrig_;
  std::auto_ptr<std::vector<float> > 	ElMvaNonTrig_;
  std::auto_ptr<std::vector<float> > 	ElMvaPresel_;
  std::auto_ptr<std::vector<float> > 	ElDEtaTkScAtVtx_;
  std::auto_ptr<std::vector<float> > 	ElDPhiTkScAtVtx_;
  std::auto_ptr<std::vector<float> > 	ElHoE_;
  std::auto_ptr<std::vector<float> > 	ElSigmaIetaIeta_;
  std::auto_ptr<std::vector<float> > 	ElSigmaIetaIetaFull5x5_;
  std::auto_ptr<std::vector<float> > 	ElEinvMinusPinv_;
  std::auto_ptr<std::vector<float> > 	ElD0_;
  std::auto_ptr<std::vector<float> > 	ElDz_;
  std::auto_ptr<std::vector<int> >   	ElExpectedMissingInnerHitCnt_;
  std::auto_ptr<std::vector<int> >   	ElPassConvVeto_;
  std::auto_ptr<std::vector<unsigned> > ElHltMatch_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoChHad_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoNeutralHad_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoIso_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoPuChHad_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoRaw_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoDbeta_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoRho_;
  std::auto_ptr<std::vector<float> > 	ElAEff_;
  
  std::auto_ptr<std::vector<float> > 	 charged_;
  std::auto_ptr<std::vector<float> > 	 photon_;
  std::auto_ptr<std::vector<float> > 	 neutral_;
  std::auto_ptr<std::vector<float> > 	 charged_Tom_;
  std::auto_ptr<std::vector<float> > 	 photon_Tom_;
  std::auto_ptr<std::vector<float> > 	 neutral_Tom_;

  //Photons
  //photon momenta
  std::auto_ptr<std::vector<float> > PhotPt_;
  std::auto_ptr<std::vector<float> > PhotEta_;
  std::auto_ptr<std::vector<float> > PhotPhi_;
  std::auto_ptr<std::vector<float> > PhotScRawE_;
  std::auto_ptr<std::vector<float> > PhotScEta_;
  std::auto_ptr<std::vector<float> > PhotScPhi_;
  
  
  //photon isolations
  std::auto_ptr<std::vector<float> > PhotIsoEcal_;
  std::auto_ptr<std::vector<float> > PhotIsoHcal_;
  std::auto_ptr<std::vector<float> > PhotIsoTk_;
  std::auto_ptr<std::vector<float> > PhotPfIsoChHad_;
  std::auto_ptr<std::vector<float> > PhotPfIsoNeutralHad_;
  std::auto_ptr<std::vector<float> > PhotPfIsoPhot_;
  std::auto_ptr<std::vector<float> > PhotPfIsoPuChHad_;
  std::auto_ptr<std::vector<float> > PhotPfIsoEcalClus_;
  std::auto_ptr<std::vector<float> > PhotPfIsoHcalClus_;
  
  //photon cluster shapes
  std::auto_ptr<std::vector<float> > PhotE3x3_;
  std::auto_ptr<std::vector<float> > PhotE1x5_;
  std::auto_ptr<std::vector<float> > PhotE1x3_;
  std::auto_ptr<std::vector<float> > PhotE2x2_;
  std::auto_ptr<std::vector<float> > PhotE2x5_;
  std::auto_ptr<std::vector<float> > PhotE5x5_;
  std::auto_ptr<std::vector<float> > PhotSigmaIetaIeta_;
  std::auto_ptr<std::vector<float> > PhotSigmaIetaIphi_;
  std::auto_ptr<std::vector<float> > PhotSigmaIphiIphi_;
  std::auto_ptr<std::vector<float> > PhotEtaWidth_;
  std::auto_ptr<std::vector<float> > PhotPhiWidth_;

  //photon preshower
  std::auto_ptr<std::vector<float> > PhotEsE_;
  std::auto_ptr<std::vector<float> > PhotEsSigmaIxIx_;
  std::auto_ptr<std::vector<float> > PhotEsSigmaIyIy_;
  std::auto_ptr<std::vector<float> > PhotEsSigmaIrIr_;
  
  //photon id (bit field)
  std::auto_ptr<std::vector<unsigned> > PhotId_;
  std::auto_ptr<std::vector<float> >    PhotHoE_;
  std::auto_ptr<std::vector<bool> >     PhotHasPixelSeed_;

  //photon timing
  std::auto_ptr<std::vector<float> > PhotTime_;
  
  ///PF Jets
  std::auto_ptr<std::vector<float> > JetAk04Pt_;
  std::auto_ptr<std::vector<float> > JetAk04Eta_;
  std::auto_ptr<std::vector<float> > JetAk04Phi_;
  std::auto_ptr<std::vector<float> > JetAk04E_;
  std::auto_ptr<std::vector<float> > JetAk04Id_;
  std::auto_ptr<std::vector<bool> >  JetAk04PuId_;
  std::auto_ptr<std::vector<float> > JetAk04PuMva_;
  std::auto_ptr<std::vector<float> > JetAk04RawPt_;
  std::auto_ptr<std::vector<float> > JetAk04RawE_;
  std::auto_ptr<std::vector<float> > JetAk04HfHadE_;
  std::auto_ptr<std::vector<float> > JetAk04HfEmE_;
  std::auto_ptr<std::vector<float> > JetAk04JetBetaClassic_;
  std::auto_ptr<std::vector<float> > JetAk04JetBeta_;
  std::auto_ptr<std::vector<float> > JetAk04JetBetaStar_;
  std::auto_ptr<std::vector<float> > JetAk04JetBetaStarClassic_;
  std::auto_ptr<std::vector<float> > JetAk04ChHadFrac_;
  std::auto_ptr<std::vector<float> > JetAk04NeutralHadAndHfFrac_;
  std::auto_ptr<std::vector<float> > JetAk04ChEmFrac_;
  std::auto_ptr<std::vector<float> > JetAk04NeutralEmFrac_;
  std::auto_ptr<std::vector<float> > JetAk04ChMult_;
  std::auto_ptr<std::vector<float> > JetAk04ConstCnt_;
  std::auto_ptr<std::vector<float> > JetAk04BTagCsv_;
  std::auto_ptr<std::vector<float> > JetAk04BTagCsvV1_;
  std::auto_ptr<std::vector<float> > JetAk04BTagCsvSLV1_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscCisvV2_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscJp_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscBjp_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscTche_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscTchp_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscSsvhe_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscSsvhp_;
  std::auto_ptr<std::vector<float> > JetAk04PartFlav_;
  std::auto_ptr<std::vector<float> > JetAk04JecUncUp_;
  std::auto_ptr<std::vector<float> > JetAk04JecUncDwn_;
  std::auto_ptr<std::vector<int> >   JetAk04ConstId_;
  std::auto_ptr<std::vector<float> > JetAk04ConstPt_;
  std::auto_ptr<std::vector<float> > JetAk04ConstEta_;
  std::auto_ptr<std::vector<float> > JetAk04ConstPhi_;
  std::auto_ptr<std::vector<float> > JetAk04ConstE_;
  std::auto_ptr<std::vector<int> >   JetAk04GenJet_;
  
  //bits
  unsigned kMuIdLoose_;
  unsigned kMuIdCustom_;
  unsigned kGlobMu_;
  unsigned kTkMu_;
  unsigned kPfMu_;

  unsigned kElec17_Elec8_;
  unsigned kMu17_Mu8_;
  unsigned kMu17_TkMu8_;

  unsigned kCutBasedElId_CSA14_50ns_V1_standalone_veto_;
  unsigned kCutBasedElId_CSA14_50ns_V1_standalone_loose_;
  unsigned kCutBasedElId_CSA14_50ns_V1_standalone_medium_;
  unsigned kCutBasedElId_CSA14_50ns_V1_standalone_tight_;
  
//  JetCorrectionUncertainty *jecUnc;


///Event objects
  edm::Handle<GenParticleCollection> genParticles_h;
  const GenParticleCollection* genParticles;
  edm::Handle<edm::View<pat::Muon> > muons;
  const edm::View<pat::Muon> * muon;
  edm::Handle<vector<pat::Electron> > electrons;
  const vector<pat::Electron>  *electron;
  edm::Handle<reco::ConversionCollection> conversions_h;
  edm::Handle<edm::View<pat::Tau> > taus;
  edm::Handle<edm::View<pat::Jet> > jets;
  const edm::View<pat::Jet> * jettt;
  edm::Handle<edm::View<pat::MET> > mets;
  const edm::View<pat::Photon>  *photons;
  edm::Handle<edm::View<reco::Vertex> >  pvHandle;
  edm ::Handle<reco::VertexCollection> vtx_h;
  const edm::View<reco::Vertex> * vtxx;
  double rhoIso;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  reco::BeamSpot beamSpot;
  edm::Handle<double> rho;
  edm::Handle<reco::GenJetCollection> genjetColl_;
///
};

using namespace std;
using namespace reco;
int ccnevent=0;
Tupel::Tupel(const edm::ParameterSet& iConfig):
  //trigger_( iConfig.getParameter< edm::InputTag >( "trigger" ) ),
  //triggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  //triggerSummaryLabel_( iConfig.getParameter< edm::InputTag >( "triggerSummaryLabel" ) ), //added by jyhan
  elecMatch_( iConfig.getParameter< std::string >( "elecMatch" ) ),
  muonMatch_( iConfig.getParameter< std::string >( "muonMatch" ) ),
  muonMatch2_( iConfig.getParameter< std::string >( "muonMatch2" ) ),
  photonSrc_(iConfig.getUntrackedParameter<edm::InputTag>("photonSrc")),
  elecSrc_(iConfig.getUntrackedParameter<edm::InputTag>("electronSrc")),
  muonSrc_(iConfig.getUntrackedParameter<edm::InputTag>("muonSrc")),
  //tauSrc_(iConfig.getUntrackedParameter<edm::InputTag>("tauSrc" )),
  jetSrc_(iConfig.getUntrackedParameter<edm::InputTag>("jetSrc" )),
  gjetSrc_(iConfig.getUntrackedParameter<edm::InputTag>("gjetSrc" )),
  metSrc_(iConfig.getUntrackedParameter<edm::InputTag>("metSrc" )),
  mSrcRho_(iConfig.getUntrackedParameter<edm::InputTag>("mSrcRho" )),
  //  CaloJet_(iConfig.getUntrackedParameter<edm::InputTag>("CalojetLabel")),
  lheSource_(iConfig.getUntrackedParameter<edm::InputTag>("lheSource")),
  genParticleSrc_(iConfig.getUntrackedParameter<edm::InputTag >("genSrc")),
  metSources(iConfig.getParameter<std::vector<edm::InputTag> >("metSource")),
  photonIdsListed_(false),
  elecIdsListed_(false),
  hltListed_(false)

  //full5x5SigmaIEtaIEtaMapToken_(consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("full5x5SigmaIEtaIEtaMap")))


{
}

Tupel::~Tupel()
{
}

void Tupel::defineBitFields(){
  DEF_BIT(TrigHlt, 0, Elec17_Elec8);
  DEF_BIT(TrigHlt, 1, Mu17_Mu8);
  DEF_BIT(TrigHlt, 2, Mu17_TkMu8);
  DEF_BIT2(TrigHltPhot, 1 , Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60);
  DEF_BIT2(TrigHltPhot, 2 , Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15);
  DEF_BIT2(TrigHltPhot, 3 , Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70);
  DEF_BIT2(TrigHltPhot, 4 , Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95);
  DEF_BIT2(TrigHltPhot, 5 , Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55);
  DEF_BIT2(TrigHltPhot, 6 , Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55);
  DEF_BIT2(TrigHltPhot, 7 , Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55);
  DEF_BIT2(TrigHltPhot, 8 , DoublePhoton85);
  DEF_BIT2(TrigHltPhot, 9 , Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 10, Photon120_R9Id90_HE10_Iso40_EBOnly_VBF);
  DEF_BIT2(TrigHltPhot, 11, Photon120_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 12, Photon120);
  DEF_BIT2(TrigHltPhot, 13, Photon135_PFMET100_NoiseCleaned);
  DEF_BIT2(TrigHltPhot, 14, Photon165_HE10);
  DEF_BIT2(TrigHltPhot, 15, Photon165_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 16, Photon175);
  DEF_BIT2(TrigHltPhot, 17, Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 18, Photon22_R9Id90_HE10_Iso40_EBOnly_VBF);
  DEF_BIT2(TrigHltPhot, 19, Photon22_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 20, Photon22);
  DEF_BIT2(TrigHltPhot, 21, Photon250_NoHE);
  DEF_BIT2(TrigHltPhot, 22, Photon30_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 23, Photon30);
  DEF_BIT2(TrigHltPhot, 24, Photon300_NoHE);
  DEF_BIT2(TrigHltPhot, 25, Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 26, Photon36_R9Id90_HE10_Iso40_EBOnly_VBF);
  DEF_BIT2(TrigHltPhot, 27, Photon36_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 28, Photon36);
  DEF_BIT2(TrigHltPhot, 29, Photon42_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon25_AND_HE10_R9Id65_Eta2_Mass15);
  DEF_BIT2(TrigHltPhot, 30, Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 31, Photon50_R9Id90_HE10_Iso40_EBOnly_VBF);
  DEF_BIT2(TrigHltPhot, 32, Photon50_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 33, Photon50);
  DEF_BIT2(TrigHltPhot, 34, Photon500);
  DEF_BIT2(TrigHltPhot, 35, Photon600);
  DEF_BIT2(TrigHltPhot, 36, Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 37, Photon75_R9Id90_HE10_Iso40_EBOnly_VBF);
  DEF_BIT2(TrigHltPhot, 38, Photon75_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 39, Photon75);
  DEF_BIT2(TrigHltPhot, 40, Photon90_CaloIdL_PFHT500);
  DEF_BIT2(TrigHltPhot, 41, Photon90_CaloIdL_PFHT600);
  DEF_BIT2(TrigHltPhot, 42, Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 43, Photon90_R9Id90_HE10_Iso40_EBOnly_VBF);
  DEF_BIT2(TrigHltPhot, 44, Photon90_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 45, Photon90);

  DEF_BIT2(TrigHltDiMu, 1 ,HLT_DoubleMu33NoFiltersNoVtx_v1);
  DEF_BIT2(TrigHltDiMu, 2 ,HLT_DoubleMu38NoFiltersNoVtx_v1);
  DEF_BIT2(TrigHltDiMu, 3 ,HLT_DoubleMu23NoFiltersNoVtxDisplaced_v1);
  DEF_BIT2(TrigHltDiMu, 4 ,HLT_DoubleMu28NoFiltersNoVtxDisplaced_v1);
  DEF_BIT2(TrigHltDiMu, 5 ,HLT_DoubleIsoMu17_eta2p1_v2);
  DEF_BIT2(TrigHltDiMu, 6 ,HLT_L2DoubleMu23_NoVertex_v1);
  DEF_BIT2(TrigHltDiMu, 7 ,HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10_v1);
  DEF_BIT2(TrigHltDiMu, 8 ,HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10_v1);
  DEF_BIT2(TrigHltDiMu, 9 ,HLT_Mu17_Mu8_DZ_v1);
  DEF_BIT2(TrigHltDiMu, 10,HLT_Mu17_Mu8_SameSign_DZ_v1);
  DEF_BIT2(TrigHltDiMu, 11,HLT_Mu20_Mu10_v1);
  DEF_BIT2(TrigHltDiMu, 12,HLT_Mu20_Mu10_DZ_v1);
  DEF_BIT2(TrigHltDiMu, 13,HLT_Mu20_Mu10_SameSign_DZ_v1);
  DEF_BIT2(TrigHltDiMu, 14,HLT_Mu27_TkMu8_v2);
  DEF_BIT2(TrigHltDiMu, 15,HLT_Mu30_TkMu11_v2);
  DEF_BIT2(TrigHltDiMu, 16,HLT_Mu40_TkMu11_v2);
  DEF_BIT2(TrigHltDiMu, 17,HLT_Mu17_TkMu8_DZ_v2);
  DEF_BIT2(TrigHltDiMu, 18,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v2);
  DEF_BIT2(TrigHltDiMu, 19,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v2);
  DEF_BIT2(TrigHltDiMu, 20,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v2);
  DEF_BIT2(TrigHltDiMu, 21,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v2);
  DEF_BIT2(TrigHltDiMu, 22,HLT_DoubleMu18NoFiltersNoVtx_v1);
  DEF_BIT2(TrigHltDiMu, 23,HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx_v2);
  DEF_BIT2(TrigHltDiMu, 24,HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx_v2);
  
  DEF_BIT2(TrigHltMu, 1 , HLT_Mu16_eta2p1_CaloMET30_v2);
  DEF_BIT2(TrigHltMu, 2 , HLT_IsoMu16_eta2p1_CaloMET30_v2);
  DEF_BIT2(TrigHltMu, 3 , HLT_IsoMu16_eta2p1_CaloMET30_LooseIsoPFTau50_Trk30_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 4 , HLT_IsoMu17_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 5 , HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2);
  DEF_BIT2(TrigHltMu, 6 , HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1_v2);
  DEF_BIT2(TrigHltMu, 7 , HLT_IsoMu17_eta2p1_MediumIsoPFTau40_Trk1_eta2p1_Reg_v2);
  DEF_BIT2(TrigHltMu, 8 , HLT_IsoMu24_eta2p1_LooseIsoPFTau20_v2);
  DEF_BIT2(TrigHltMu, 9 , HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07_v2);
  DEF_BIT2(TrigHltMu, 10, HLT_IsoMu20_eta2p1_TriCentralPFJet30_v2);
  DEF_BIT2(TrigHltMu, 11, HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30_v2);
  DEF_BIT2(TrigHltMu, 12, HLT_IsoMu20_v2);
  DEF_BIT2(TrigHltMu, 13, HLT_IsoMu20_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 14, HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v2);
  DEF_BIT2(TrigHltMu, 15, HLT_IsoMu24_eta2p1_TriCentralPFJet30_v2);
  DEF_BIT2(TrigHltMu, 16, HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v2);
  DEF_BIT2(TrigHltMu, 17, HLT_IsoMu24_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 18, HLT_IsoMu27_v2);
  DEF_BIT2(TrigHltMu, 19, HLT_IsoTkMu20_v2);
  DEF_BIT2(TrigHltMu, 20, HLT_IsoTkMu20_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 21, HLT_IsoTkMu24_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 22, HLT_IsoTkMu27_v2);
  DEF_BIT2(TrigHltMu, 23, HLT_L2Mu10_v1);
  DEF_BIT2(TrigHltMu, 24, HLT_L2Mu10_NoVertex_NoBPTX3BX_NoHalo_v1);
  DEF_BIT2(TrigHltMu, 25, HLT_L2Mu10_NoVertex_NoBPTX_v1);
  DEF_BIT2(TrigHltMu, 26, HLT_L2Mu35_NoVertex_3Sta_NoBPTX3BX_NoHalo_v1);
  DEF_BIT2(TrigHltMu, 27, HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX_NoHalo_v1);
  DEF_BIT2(TrigHltMu, 28, HLT_Mu20_v1);
  DEF_BIT2(TrigHltMu, 29, HLT_TkMu20_v2);
  DEF_BIT2(TrigHltMu, 30, HLT_Mu24_eta2p1_v1);
  DEF_BIT2(TrigHltMu, 31, HLT_TkMu24_eta2p1_v2);
  DEF_BIT2(TrigHltMu, 32, HLT_Mu27_v1);
  DEF_BIT2(TrigHltMu, 33, HLT_TkMu27_v2);
  DEF_BIT2(TrigHltMu, 34, HLT_Mu50_v1);
  DEF_BIT2(TrigHltMu, 35, HLT_Mu55_v1);
  DEF_BIT2(TrigHltMu, 36, HLT_Mu45_eta2p1_v1);
  DEF_BIT2(TrigHltMu, 37, HLT_Mu50_eta2p1_v1);
  DEF_BIT2(TrigHltMu, 38, HLT_Mu8_TrkIsoVVL_v2);
  DEF_BIT2(TrigHltMu, 39, HLT_Mu17_TrkIsoVVL_v2);
  DEF_BIT2(TrigHltMu, 40, HLT_Mu24_TrkIsoVVL_v2);
  DEF_BIT2(TrigHltMu, 41, HLT_Mu34_TrkIsoVVL_v2);
  DEF_BIT2(TrigHltMu, 42, HLT_Mu8_v1);
  DEF_BIT2(TrigHltMu, 43, HLT_Mu17_v1);
  DEF_BIT2(TrigHltMu, 44, HLT_Mu24_v1);
  DEF_BIT2(TrigHltMu, 45, HLT_Mu34_v1);	 
  
  DEF_BIT(MuId, 0, MuIdLoose);
  DEF_BIT_L(MuId, 3, MuIdCustom, "Mu Id: isGlobalMuon\n"
			  "&& isPFMuon\n"
			  "&& normChi2 < 10\n"
			  "&& muonHits > 0 && nMatches > \n"
			  "&& dB < 0.2 && dZ < 0.5\n"
			  "&& pixelHits > 0 && trkLayers > 5");
  DEF_BIT(MuType, 0, GlobMu);
  DEF_BIT(MuType, 1, TkMu);
  DEF_BIT(MuType, 2, PfMu);

  DEF_BIT(ElId, 0, CutBasedElId_CSA14_50ns_V1_standalone_veto);
  DEF_BIT(ElId, 1, CutBasedElId_CSA14_50ns_V1_standalone_loose);
  DEF_BIT(ElId, 2, CutBasedElId_CSA14_50ns_V1_standalone_medium);
  DEF_BIT(ElId, 3, CutBasedElId_CSA14_50ns_V1_standalone_tight);

}

void Tupel::readEvent(const edm::Event& iEvent){
  *EvtNum_        = iEvent.id().event();
  *EvtRunNum_     = iEvent.id().run();
  *EvtLumiNum_    = iEvent.luminosityBlock();
  *EvtBxNum_      = iEvent.bunchCrossing();
  *EvtIsRealData_ = iEvent.isRealData();

  const pat::helper::TriggerMatchHelper matchHelper;	
  iEvent.getByLabel(genParticleSrc_, genParticles_h);
  genParticles  = genParticles_h.failedToGet () ? 0 : &*genParticles_h;
  
  // get muon collection
  iEvent.getByLabel(muonSrc_,muons);
  muon = muons.failedToGet () ? 0 : &*muons;

  
  // get electron collection
  iEvent.getByLabel(elecSrc_,electrons);
  electron = electrons.failedToGet () ? 0 :  &*electrons;
			      
 // edm::Handle<reco::GsfElectronCollection> els_h;
 // iEvent.getByLabel("gsfElectrons", els_h);			       
  iEvent.getByLabel(InputTag("reducedEgamma","reducedConversions"), conversions_h);  
  
  // get tau collection 
  iEvent.getByLabel(tauSrc_,taus);
					  
  // get jet collection
  iEvent.getByLabel(jetSrc_,jets);
  jettt = jets.failedToGet () ? 0 : &*jets ;
  
  // get met collection  
  iEvent.getByLabel(metSrc_,mets);
  
  // get photon collection  
  edm::Handle<edm::View<pat::Photon> > hPhotons;
  iEvent.getByLabel(photonSrc_, hPhotons);
  photons = hPhotons.failedToGet () ? 0 :  &*hPhotons;

  //get Gen jets
  iEvent.getByLabel(gjetSrc_, genjetColl_);
  
  iEvent.getByLabel("goodOfflinePrimaryVertices", pvHandle);
  vtxx = pvHandle.failedToGet () ? 0 : &*pvHandle ;
			  
  iEvent.getByLabel("goodOfflinePrimaryVertices", vtx_h); 
//  if(vtxx){
//    int nvtx=vtx_h->size();
//    if(nvtx==0) return;
//    reco::VertexRef primVtx(vtx_h,0);
//  }

  edm::Handle<double>  rho_;
  iEvent.getByLabel(mSrcRho_, rho_);
  rhoIso=99;
  if(!rho_.failedToGet())rhoIso = *rho_;
  iEvent.getByLabel("offlineBeamSpot", beamSpotHandle);
  if(!beamSpotHandle.failedToGet()) beamSpot = *beamSpotHandle;
  iEvent.getByLabel(mSrcRho_,rho);
}

void Tupel::processMET(const edm::Event& iEvent){
   for(unsigned int imet=0;imet<metSources.size();imet++){
    Handle<View<pat::MET> > metH;
    iEvent.getByLabel(metSources[imet], metH);
    if(!metH.isValid())continue;
    //cout<<"MET"<<imet<<"  "<<metSources[imet]<<"  "<<metH->ptrAt(0)->pt()<<endl;

    METPt_->push_back(metH->ptrAt(0)->pt()); 
    METPx_->push_back(metH->ptrAt(0)->px()); 
    METPy_->push_back(metH->ptrAt(0)->py()); 
    METPz_->push_back(metH->ptrAt(0)->pz()); 
    METE_->push_back(metH->ptrAt(0)->energy()); 
    METsigx2_->push_back(metH->ptrAt(0)->getSignificanceMatrix()(0,0)); 
    METsigxy_->push_back(metH->ptrAt(0)->getSignificanceMatrix()(0,1)); 
    METsigy2_->push_back(metH->ptrAt(0)->getSignificanceMatrix()(1,1)); 
    METsig_->push_back(metH->ptrAt(0)->significance()); 
    //Output object in EDM format
    //std::auto_ptr<llvvMet> metOut(new llvvMet());
    //llvvMet& met = *metOut;

    //////////////////////////////////

    // met.SetPxPyPzE(metH->ptrAt(0)->px(), metH->ptrAt(0)->py(), metH->ptrAt(0)->pz(), metH->ptrAt(0)->energy());
    //met.sigx2 = metH->ptrAt(0)->getSignificanceMatrix()(0,0);
    //met.sigxy = metH->ptrAt(0)->getSignificanceMatrix()(0,1);
    //met.sigy2 = metH->ptrAt(0)->getSignificanceMatrix()(1,1);
    //met.sig   = metH->ptrAt(0)->significance();

    //iEvent.put(metOut, metSources[imet].label()); //save the object to the EvtNum here, to keep it in the loop
  }
}

void Tupel::processVtx(){
  if(vtxx){
    for (edm::View<reco::Vertex>::const_iterator vtx = pvHandle->begin(); vtx != pvHandle->end(); ++vtx){
      if (vtx->isValid() && !vtx->isFake()) ++(*EvtVtxCnt_);
    }
  }
}

void Tupel::processPu(const edm::Event& iEvent){
  Handle<std::vector< PileupSummaryInfo > >  PupInfo;
  iEvent.getByLabel("addPileupInfo", PupInfo);
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
    //////TO CHECK
    *EvtPuCntTruth_ = npT;
    *EvtPuCnt_      = npIT;
  }
  else {
    *EvtPuCntTruth_ = -2.; 
    *EvtPuCnt_ = -2.;
  }
}

void Tupel::processGenParticles(const edm::Event& iEvent){
  const std::vector<reco::GenParticle> & gen = *genParticles_h;
  for (size_t i=0; i<genParticles->size(); ++i){
    TLorentzVector genR1DressLep1(0,0,0,0);
    //      TLorentzVector genPho(0,0,0,0); 
    int st = gen[i].status();
    int id = gen[i].pdgId();


    if(gen[i].numberOfMothers()){
      //        if (st!=3 && fabs(id)!=13&& fabs(id)!=11 && fabs(id)!=22 && fabs(id)!=23) continue;
      // if(abs(st)==13 ||abs(st)==12||abs(st)==11||abs(st)==23 ||abs(st)==22||abs(st)==21||abs(st)==61 )cout<<"AAA "<<gen[i].numberOfMothers() <<"  "<< gen[i].mother()->pdgId()<<"  "<< gen[i].pdgId()<<"  "<<st<<"  "<<gen[i].px()<<"  "<<gen[i].py()<<"  "<<gen[i].pz()<<"  "<<gen[i].energy()<<endl;
      if (abs(st)==23 ||abs(st)==22||abs(st)==21||abs(st)==61 ||abs(st)==3 ){
	TLorentzVector genLep3(0,0,0,0);
	if(abs(st)!=21)genLep3.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
	if(abs(st)==21)genLep3.SetPxPyPzE(0.001,0.001,gen[i].pz(),gen[i].energy());
	GLepSt3Pt_->push_back(genLep3.Pt());
	GLepSt3Eta_->push_back(eta(genLep3));
	GLepSt3Phi_->push_back(genLep3.Phi());
	GLepSt3E_->push_back(genLep3.Energy());
	GLepSt3Mother0Id_->push_back(gen[i].mother()->pdgId());
	GLepSt3Id_->push_back(id);
	GLepSt3St_->push_back(st);
      }
      GLepSt3MotherCnt_->push_back(gen[i].numberOfMothers());
      /* if(abs(id)==15){
      //cout<<gen[i].numberOfMothers() <<"  "<< gen[i].mother()->pdgId()<<"  "<< gen[i].pdgId()<<"  "<<st<<endl;
      n_tau++;
      }*/
      if(gen[i].numberOfMothers() ==1 && gen[i].mother()->pdgId() != id){
	//if(abs(id)==15)cout<<"DEAD"<<endl;
	continue;
      }
      if (st==1 && (abs(id)==13||abs(id)==11 || abs(id)==15 ||abs(id)==12||abs(id)==14||abs(id)==16) /*&& gen[i].pt() > 0.1 && fabs(gen[i].eta())<3.0*/){

	TLorentzVector genLep1(0,0,0,0);
	genLep1.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
	TLorentzVector genR1Pho1(0,0,0,0);

	edm::Handle<std::vector<reco::GenParticle> > genpart2;//DONT know why we Need to handle another collection
	iEvent.getByLabel(genParticleSrc_, genpart2);
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
		GLepClosePhotPt_->push_back(thisPho1.Pt());
		GLepClosePhotEta_->push_back(thisPho1.Eta());
		GLepClosePhotPhi_->push_back(thisPho1.Phi());
		GLepClosePhotE_->push_back(thisPho1.Energy());
		GLepClosePhotId_->push_back(gen2[j].pdgId());
		GLepClosePhotMother0Id_->push_back(fabs(gen2[j].mother()->pdgId()));
		GLepClosePhotMotherCnt_->push_back(gen2[j].numberOfMothers());
		GLepClosePhotSt_->push_back(gen2[j].status());
	      }
	    }
	  }
	}

	genR1DressLep1 = genLep1+genR1Pho1;
	GLepDr01Pt_->push_back(genR1DressLep1.Pt());
	GLepDr01Eta_->push_back(genR1DressLep1.Eta());
	GLepDr01Phi_->push_back(genR1DressLep1.Phi());
	GLepDr01E_->push_back(genR1DressLep1.Energy());
	GLepDr01Id_->push_back(id);
	GLepDr01MomId_->push_back(id);
	GLepDr01St_->push_back(st);

	GLepBarePt_->push_back(genLep1.Pt());
	GLepBareEta_->push_back(genLep1.Eta());
	GLepBarePhi_->push_back(genLep1.Phi());
	GLepBareE_->push_back(genLep1.Energy());
	GLepBareId_->push_back(id);
	GLepBareMomId_->push_back(id);
	GLepBareSt_->push_back(st);
      }
    }
  }
}

void Tupel::processGenJets(const edm::Event& iEvent){
  //matrix element info
  Handle<LHEEventProduct> lheH;
  iEvent.getByLabel(lheSource_,lheH);//to be modularized!!!
  if(lheH.isValid()) *GNup_ = lheH->hepeup().NUP;
  if(!genjetColl_.failedToGet()){
    const reco::GenJetCollection & genjet = *genjetColl_;
    for(unsigned int k=0; k < genjet.size(); ++k){
      GJetAk04Pt_->push_back(genjet[k].pt());
      GJetAk04Eta_->push_back(genjet[k].eta());
      GJetAk04Phi_->push_back(genjet[k].phi());
      GJetAk04E_->push_back(genjet[k].energy());
      //double isChargedJet=false;
      //double chargedFraction = 0.;
      //std::vector<const GenParticle*> mcparticles = genjet[k].getGenConstituents();
      //for(std::vector <const GenParticle*>::const_iterator thepart =mcparticles.begin();thepart != mcparticles.end(); ++ thepart ) {
      //  if ( (**thepart).charge()!=0 ){
      //isChargedJet=true;
      //    chargedFraction += (**thepart).pt();
      //  }
      //}
      //if ( chargedFraction == 0 ) cout << " is chargeid: " << isChargedJet << "   " << chargedFraction/genjet[k].pt()<< endl;
      //GJetAk04ChargedFraction_->push_back(chargedFraction/genjet[k].pt());
      GJetAk04ConstCnt_->push_back(genjet[k].numberOfDaughters());
      /*if(genjet[k].numberOfDaughters()>0){
	for(unsigned int idx =0; idx<genjet[k].numberOfDaughters();idx++){
	//cout<<genjet[k].numberOfDaughters()<<" GEN AHMEEEEET "<<idx<<"  "<<genjet[k].daughter(idx)->pdgId()<<"  "<<endl;
	GJetAk04ConstId->push_back(genjet[k].daughter(idx)->pdgId());
	GJetAk04ConstPt->push_back(genjet[k].daughter(idx)->pt());
	GJetAk04ConstEta->push_back(genjet[k].daughter(idx)->eta());
	GJetAk04ConstPhi->push_back(genjet[k].daughter(idx)->phi());
	GJetAk04ConstE->push_back(genjet[k].daughter(idx)->energy();)
	}
	}*/
    }
  }
}

void Tupel::processPdfInfo(const edm::Event& iEvent){
  edm::Handle<GenEventInfoProduct> genEventInfoProd;
  if (iEvent.getByLabel("generator", genEventInfoProd)) {
    if (genEventInfoProd->hasBinningValues()){
      *GBinningValue_ = genEventInfoProd->binningValues()[0];
    }
    *EvtWeights_ = genEventInfoProd->weights();
  }
  /// now get the PDF information
  edm::Handle<GenEventInfoProduct> pdfInfoHandle;
  if (iEvent.getByLabel("generator", pdfInfoHandle)) {
    if (pdfInfoHandle->pdf()) {
      GPdfId1_->push_back(pdfInfoHandle->pdf()->id.first);
      GPdfId2_->push_back(pdfInfoHandle->pdf()->id.second);
      GPdfx1_->push_back(pdfInfoHandle->pdf()->x.first);
      GPdfx2_->push_back(pdfInfoHandle->pdf()->x.second);
      //pdfInfo_->push_back(pdfInfoHandle->pdf()->xPDF.first);
      //dfInfo_->push_back(pdfInfoHandle->pdf()->xPDF.second);
      GPdfScale_->push_back(pdfInfoHandle->pdf()->scalePDF);
    }   
  }   
}

void Tupel::processTrigger(const edm::Event& iEvent){

  int ntrigs;
  std::vector<std::string> trigname;
  std::vector<bool> trigaccept;
  edm::Handle< edm::TriggerResults > HLTResHandle;
  edm::InputTag HLTTag = edm::InputTag( "TriggerResults", "", "HLT");
  iEvent.getByLabel(HLTTag, HLTResHandle);
  if ( HLTResHandle.isValid() && !HLTResHandle.failedToGet() ) {
    edm::RefProd<edm::TriggerNames> trigNames( &(iEvent.triggerNames( *HLTResHandle )) );
    ntrigs = (int)trigNames->size();
    for (int i = 0; i < ntrigs; i++) {
      trigname.push_back(trigNames->triggerName(i));
      trigaccept.push_back(HLTResHandle->accept(i));
      if (trigaccept[i]){
	if(trigname[i].find("HLT_Mu17_Mu8")!=std::string::npos) *TrigHlt_ |= kMu17_Mu8_;
	if(trigname[i].find("HLT_Mu17_TkMu8")!=std::string::npos) *TrigHlt_ |= kMu17_TkMu8_;
	if(trigname[i].find(
			  "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL"
			    )!=std::string::npos) *TrigHlt_ |= kElec17_Elec8_;
	fillTrig(std::string(trigname[i]));
      }
    }
    if(!hltListed_){
      std::ofstream f("trigger_list.txt");
      for (int i = 0; i < ntrigs; i++) {
	f << trigname[i] << "\n";
      }
      hltListed_ = true;
    }
  }
}

void Tupel::fillTrig(const std::string& trigname){
  for(std::map<std::string, ULong64_t>::const_iterator it = TrigHltPhotMap_.begin();
      it != TrigHltPhotMap_.end(); ++it){
    if(trigname.find(it->first)!=std::string::npos) *TrigHltPhot_ |= it->second;
  }
  
  for(std::map<std::string, ULong64_t>::const_iterator it = TrigHltMuMap_.begin();
      it != TrigHltMuMap_.end(); ++it){
    if(trigname.find(it->first)!=std::string::npos) *TrigHltMu_ |= it->second;
  }

  for(std::map<std::string, ULong64_t>::const_iterator it = TrigHltDiMuMap_.begin();
      it != TrigHltDiMuMap_.end(); ++it){
    if(trigname.find(it->first)!=std::string::npos) *TrigHltDiMu_ |= it->second;
  }
}

void Tupel::processMuons(){
  double MuFill = 0;
  
  for (unsigned int j = 0; j < muons->size(); ++j){
    const edm::View<pat::Muon> & mu = *muons;
    if(mu[j].isGlobalMuon()){ 
      //const pat::TriggerObjectRef trigRef( matchHelper.triggerMatchObject( muons,j,muonMatch_, iEvent, *triggerEvent ) );
      //if ( trigRef.isAvailable() && trigRef.isNonnull() ) {
      //  Mu17_Mu8_Matched=1;
      //	}
      //	const pat::TriggerObjectRef trigRef2( matchHelper.triggerMatchObject( muons,j,muonMatch2_, iEvent, *triggerEvent ) );
      //	if ( trigRef2.isAvailable() && trigRef2.isNonnull() ) {
      //	  Mu17_TkMu8_Matched=1;
      //	}
	    //TODO: filled MuHltMatch
      //patMuonIdMedium_->push_back(mu[j].isMediumMuon()); Requires CMSSW >= 4_7_2
      if(vtxx){
	unsigned bit = 0;
	unsigned muonTightIds = 0;
	for (edm::View<reco::Vertex>::const_iterator vtx = pvHandle->begin(); vtx != pvHandle->end(); ++vtx){
	  if(vtx->isValid() && !vtx->isFake() && mu[j].isTightMuon(*vtx)){
	    muonTightIds |= (1 <<bit);
	  }
	  ++bit;
	  if(bit > 31) break;
	}
	MuIdTight_->push_back(muonTightIds);
      }
	  
	  
      //MuFill=1;
      MuPt_->push_back(mu[j].pt());
      MuEta_->push_back(mu[j].eta());
      MuPhi_->push_back(mu[j].phi());
      MuE_->push_back(mu[j].energy());
      MuCh_->push_back(mu[j].charge());
      MuVtxZ_->push_back(mu[j].vz());
      MuDxy_->push_back(mu[j].dB());
      
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
      MuTkNormChi2_->push_back(normChi2);
      MuTkHitCnt_->push_back(muonHits);
      MuMatchedStationCnt_->push_back(nMatches);
      MuDz_->push_back(dZ);
      MuPixelHitCnt_->push_back(pixelHits);
      MuTkLayerCnt_->push_back(trkLayers);
	
      bool customMuId =	( mu[j].isGlobalMuon()
			  && mu[j].isPFMuon()
			  && normChi2<10
			  && muonHits>0 && nMatches>1
			  && mu[j].dB()<0.2 && dZ<0.5
			  && pixelHits>0 && trkLayers>5 );
      unsigned muId = 0;
      if(mu[j].isLooseMuon()) muId |= kMuIdLoose_;
      //if(mu[j].isMediumMuon()) muId |= kMuIdMedium_;
      if(customMuId) muId |= kMuIdCustom_;
      MuId_->push_back(muId);
      
      //      unsigned muHltMath = 0;
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
      MuIsoRho_->push_back(dbeta);
	  
      // pf Isolation variables
      double chargedHadronIso = mu[j].pfIsolationR04().sumChargedHadronPt;
      double chargedHadronIsoPU = mu[j].pfIsolationR04().sumPUPt;  
      double neutralHadronIso  = mu[j].pfIsolationR04().sumNeutralHadronEt;
      double photonIso  = mu[j].pfIsolationR04().sumPhotonEt;
      double a=0.5;  
      // OPTION 1: DeltaBeta corrections for iosolation
      float RelativeIsolationDBetaCorr = (chargedHadronIso + std::max(photonIso+neutralHadronIso - 0.5*chargedHadronIsoPU,0.))/std::max(a, mu[j].pt());
      MuPfIso_->push_back(RelativeIsolationDBetaCorr);
      unsigned muType = 0;
      if(mu[j].isGlobalMuon()) muType |= kGlobMu_;
      if(mu[j].isTrackerMuon()) muType |= kTkMu_;
      if(mu[j].isPFMuon()) muType |= kPfMu_;
      MuIsoTkIsoAbs_->push_back(mu[j].isolationR03().sumPt);
      MuIsoTkIsoRel_->push_back(mu[j].isolationR03().sumPt/mu[j].pt());
      MuIsoCalAbs_->push_back(mu[j].isolationR03().emEt + mu[j].isolationR03().hadEt);
      MuIsoCombRel_->push_back((mu[j].isolationR03().sumPt+mu[j].isolationR03().hadEt)/mu[j].pt());
      MuPfIsoChHad_->push_back(mu[j].pfIsolationR03().sumChargedHadronPt);
      MuPfIsoNeutralHad_->push_back(mu[j].pfIsolationR03().sumNeutralHadronEt);
      MuPfIsoRawRel_->push_back((mu[j].pfIsolationR03().sumChargedHadronPt+mu[j].pfIsolationR03().sumNeutralHadronEt)/mu[j].pt());
      MuFill++;
    }
  }
}

void Tupel::processElectrons(){
  int ElecFill=0;
  auto_ptr<vector<pat::Electron> > electronColl( new vector<pat::Electron> (*electrons) );
  for (unsigned int j=0; j < electronColl->size();++j){
    pat::Electron & el = (*electronColl)[j];

    double dEtaIn_;
    double dPhiIn_;
    double hOverE_;
    double sigmaIetaIeta_;
    double full5x5_sigmaIetaIeta_;
    //double relIsoWithDBeta_;
    double ooEmooP_;
    double d0_ = -99.;
    double dz_ = -99.;
    int   expectedMissingInnerHits_;
    int   passConversionVeto_;     

    std::vector<std::pair<std::string,float> > idlist = el.electronIDs();
    if(!elecIdsListed_) {
      std::ofstream f("electron_id_list.txt");
      f << "Autogenerated file\n\n"
	"Supported electron ids:\n"; 
      for (unsigned k  = 0 ; k < idlist.size(); ++k){
	f << idlist[k].first << ": " << idlist[k].second  << "\n";
      }
      f.close();
      elecIdsListed_ = true;
    }
    unsigned elecid = 0;
      
    if(el.electronID(std::string("cutBasedElectronID-CSA14-50ns-V1-standalone-veto"))) elecid |= kCutBasedElId_CSA14_50ns_V1_standalone_veto_;
    if(el.electronID(std::string("cutBasedElectronID-CSA14-50ns-V1-standalone-loose"))) elecid |= kCutBasedElId_CSA14_50ns_V1_standalone_loose_;
    if(el.electronID(std::string("cutBasedElectronID-CSA14-50ns-V1-standalone-medium"))) elecid |= kCutBasedElId_CSA14_50ns_V1_standalone_medium_;
    if(el.electronID(std::string("cutBasedElectronID-CSA14-50ns-V1-standalone-tight"))) elecid |= kCutBasedElId_CSA14_50ns_V1_standalone_tight_;
    ElId_->push_back(elecid);
      
    dEtaIn_ = el.deltaEtaSuperClusterTrackAtVtx();
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

    if(vtx_h->size() > 0){
      d0_ = (-1) * el.gsfTrack()->dxy((*vtx_h)[0].position() );
      dz_ = el.gsfTrack()->dz( (*vtx_h)[0].position() );
    }



    //     expectedMissingInnerHits_ = el.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits();//MISSING!!
    passConversionVeto_ = false;
    if( beamSpotHandle.isValid() && conversions_h.isValid()) {
      passConversionVeto_ = !ConversionTools::hasMatchedConversion(el,conversions_h,
								   beamSpotHandle->position());
    }else{
      printf("\n\nERROR!!! conversions not found!!!\n");
    }


    //cout<<dEtaIn_<<"  "<<dPhiIn_<<"  "<<hOverE_<<"  "<<sigmaIetaIeta_<<"  "<<full5x5_sigmaIetaIeta_<<"  "<<ooEmooP_<<"  "<< d0_<<"  "<< dz_<<"  "<<expectedMissingInnerHits_<<"  "<<passConversionVeto_<<endl;

    ElDEtaTkScAtVtx_->push_back(dEtaIn_);
    ElDPhiTkScAtVtx_->push_back(dPhiIn_);
    ElHoE_->push_back(hOverE_);
    ElSigmaIetaIeta_->push_back(sigmaIetaIeta_);
    ElSigmaIetaIetaFull5x5_->push_back(full5x5_sigmaIetaIeta_);
    ElEinvMinusPinv_->push_back(ooEmooP_);
    ElD0_->push_back(d0_);
    ElDz_->push_back(dz_);
    ElExpectedMissingInnerHitCnt_->push_back(expectedMissingInnerHits_);
    ElExpectedMissingInnerHitCnt_->push_back(passConversionVeto_);     

    int hltMatch = 0;
      
    ElHltMatch_->push_back(hltMatch);//no matching yet...BB 
    const string mvaTrigV0 = "mvaTrigV0";
    const string mvaNonTrigV0 = "mvaNonTrigV0";
      
    ElPt_->push_back(el.pt());
    ElEta_->push_back(el.eta());
      
    ElEtaSc_->push_back(el.superCluster()->eta());
    ElPhi_->push_back(el.phi());	
    ElE_->push_back(el.energy());
    ElCh_->push_back(el.charge());

    double aeff = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, el.superCluster()->eta(), ElectronEffectiveArea::kEleEAData2012);
    ElAEff_->push_back(aeff);
      
    const double chIso03_ = el.chargedHadronIso();
    const double nhIso03_ = el.neutralHadronIso();
    const double phIso03_ = el.photonIso();
    const double puChIso03_= el.puChargedHadronIso();
    ElPfIsoChHad_->push_back(chIso03_);
    ElPfIsoNeutralHad_->push_back(nhIso03_);
    ElPfIsoIso_->push_back(phIso03_);
    ElPfIsoPuChHad_->push_back(puChIso03_);
    ElPfIsoRaw_->push_back(( chIso03_ + nhIso03_ + phIso03_ ) / el.pt());
    ElPfIsoDbeta_->push_back(( chIso03_ + max(0.0, nhIso03_ + phIso03_ - 0.5*puChIso03_) )/ el.pt());

    *EvtFastJetRho_ =  rhoIso;
    double rhoPrime = std::max(0., rhoIso);
    
    ElPfIsoRho_->push_back(( chIso03_ + max(0.0, nhIso03_ + phIso03_ - rhoPrime*(aeff)) )/ el.pt());
      
    bool myTrigPresel = false;
    if(fabs(el.superCluster()->eta()) < 1.479){
      if(el.sigmaIetaIeta() < 0.014 &&
	 el.hadronicOverEm() < 0.15 &&
	 el.dr03TkSumPt()/el.pt() < 0.2 &&
	 el.dr03EcalRecHitSumEt()/el.pt() < 0.2 &&
	 el.dr03HcalTowerSumEt()/el.pt() < 0.2 /*&&
						 el.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits() == 0*/)
	myTrigPresel = true;
    }
    else {
      if(el.sigmaIetaIeta() < 0.035 &&
	 el.hadronicOverEm() < 0.10 &&
	 el.dr03TkSumPt()/el.pt() < 0.2 &&
	 el.dr03EcalRecHitSumEt()/el.pt() < 0.2 &&
	 el.dr03HcalTowerSumEt()/el.pt() < 0.2 /*&&
						 el.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits() == 0*/)
	myTrigPresel = true;
    }     
    ElMvaPresel_->push_back(myTrigPresel);
    ElecFill++; 
  }
}    

void Tupel::processJets(){
  //double PFjetFill=0;
  double chf = 0;
  double nhf = 0;
  double cemf = 0;
  double nemf = 0;
  double cmult = 0;
  double nconst = 0;


  for ( unsigned int i=0; i<jets->size(); ++i ) {
    const pat::Jet & jet = jets->at(i);
       
    JetAk04PuMva_->push_back(jet.userFloat("pileupJetId:fullDiscriminant"));
    chf = jet.chargedHadronEnergyFraction();
    nhf = (jet.neutralHadronEnergy()+jet.HFHadronEnergy())/jet.correctedJet(0).energy();
    cemf = jet.chargedEmEnergyFraction();
    nemf = jet.neutralEmEnergyFraction();
    cmult = jet.chargedMultiplicity();
    nconst = jet.numberOfDaughters();
      
    // cout<<"jet.bDiscriminator(combinedSecondaryVertexBJetTags)=  "<<jet.bDiscriminator("combinedSecondaryVertexBJetTags")<<endl;
    //  cout<<"jet.bDiscriminator(combinedSecondaryVertexV1BJetTags)=  "<<jet.bDiscriminator("combinedSecondaryVertexV1BJetTags")<<endl;
    //  cout<<"jet.bDiscriminator(combinedSecondaryVertexSoftPFLeptonV1BJetTags)=  "<<jet.bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags")<<endl;
    JetAk04BTagCsv_->push_back(jet.bDiscriminator("combinedSecondaryVertexBJetTags"));
    JetAk04BTagCsvV1_->push_back(jet.bDiscriminator("combinedSecondaryVertexV1BJetTags"));
    JetAk04BTagCsvSLV1_->push_back(jet.bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags"));
    JetAk04BDiscCisvV2_->push_back(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    JetAk04BDiscJp_->push_back(jet.bDiscriminator("pfJetProbabilityBJetTags"));
    JetAk04BDiscBjp_->push_back(jet.bDiscriminator("pfJetBProbabilityBJetTags"));
    JetAk04BDiscTche_->push_back(jet.bDiscriminator("pfTrackCountingHighEffBJetTags"));
    JetAk04BDiscTchp_->push_back(jet.bDiscriminator("pfTrackCountingHighPurBJetTags"));
    JetAk04BDiscSsvhe_->push_back(jet.bDiscriminator("pfSimpleSecondaryVertexHighEffBJetTags"));
    JetAk04BDiscSsvhp_->push_back(jet.bDiscriminator("pfSimpleSecondaryVertexHighPurBJetTags"));
    JetAk04PartFlav_->push_back(jet.partonFlavour());
      
    JetAk04E_->push_back(jet.energy());
    JetAk04Pt_->push_back(jet.pt());
    JetAk04Eta_->push_back(jet.eta());
    JetAk04Phi_->push_back(jet.phi());
    JetAk04RawPt_->push_back(jet.correctedJet(0).pt());
    JetAk04RawE_->push_back(jet.correctedJet(0).energy());
    JetAk04HfHadE_->push_back(jet.HFHadronEnergy());
    JetAk04HfEmE_->push_back(jet.HFEMEnergy());
    JetAk04ChHadFrac_->push_back(chf);
    JetAk04NeutralHadAndHfFrac_->push_back(nhf);
    JetAk04ChEmFrac_->push_back(cemf);
    JetAk04NeutralEmFrac_->push_back(nemf);
    JetAk04ChMult_->push_back(cmult);
    JetAk04ConstCnt_->push_back(nconst);
  
    for(unsigned int idx =0; idx<jet.numberOfDaughters();idx++){
      //cout<<jet.numberOfDaughters()<<" RECO AHMEEEEET "<<idx<<"  "<<jet.daughter(idx)->pdgId()<<"  "<<endl;
      JetAk04ConstId_->push_back(jet.daughter(idx)->pdgId());
      JetAk04ConstPt_->push_back(jet.daughter(idx)->pt());
      JetAk04ConstEta_->push_back(jet.daughter(idx)->eta());
      JetAk04ConstPhi_->push_back(jet.daughter(idx)->phi());
      JetAk04ConstE_->push_back(jet.daughter(idx)->energy());


    }
    //TODO: insert correct value
    double unc = 1.;
    JetAk04JecUncUp_->push_back(unc);
    JetAk04JecUncDwn_->push_back(unc);
    double tempJetID=0;
    if( abs(jet.eta())<2.4){
      if(chf>0 && nhf<0.99 && cmult>0.0 && cemf<0.99 && nemf<0.99 && nconst>1) tempJetID=1;
      if((chf>0)&&(nhf<0.95)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.95)&&(nconst>1)) tempJetID=2; 
      if((chf>0)&&(nhf<0.9)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.9)&&(nconst>1)) tempJetID=3;
    }
    if( abs(jet.eta())>=2.4){
      if ((nhf<0.99)&&(nemf<0.99)&&(nconst>1))tempJetID=1;
      if ((nhf<0.95)&&(nemf<0.95)&&(nconst>1))tempJetID=2; 
      if ((nhf<0.9)&&(nemf<0.9)&&(nconst>1))tempJetID=3;
    }
    JetAk04Id_->push_back(tempJetID);//ala 
    //PFjetFill++;
      
    if(!*EvtIsRealData_){
      int genJetIdx = -1; //code for not matched jets
      if (jet.genJet()){
	//	genJetIdx = jet.genJetFwdRef().key();
	for(genJetIdx = 0; (unsigned) genJetIdx < genjetColl_->size();++genJetIdx){
	  if(&(*genjetColl_)[genJetIdx] == jet.genJet()) break; 
	}
	genJetIdx = jet.genJetFwdRef().backRef().key();
	assert(jet.genJet() == &(*genjetColl_)[genJetIdx]);
	//	if((unsigned)genJetIdx == genjetColl_->size()){
	//	  genJetIdx = -2;
	//	  std::cerr << "Matched gen jet not found!\n";
	//	} else{
	//	  double dphi = fabs(jet.phi()-(*genjetColl_)[genJetIdx].phi());
	//	  if(dphi > pi ) dphi = 2*pi - dphi;
	//	  std::cout << "Jet matching check: R = "
	//		    << sqrt(std::pow(jet.eta()-(*genjetColl_)[genJetIdx].eta(),2)
	//			    + std::pow(dphi,2))
	//		    << "\tPt ratio = "
	//		    << jet.pt() / (*genjetColl_)[genJetIdx].pt()
	//		    << "\t" << jet.pt() / jet.genJet()->pt()
	//		    << "\t key: " << genJetIdx
	//		    << "\t coll size: " <<  genjetColl_->size()
	//		    << "\t" << hex << jet.genJet()
	//		    << "\t" << &((*genjetColl_)[genJetIdx]) << dec
	//		    << "\tIndices: " << genJetIdx << ", " << jet.genJetFwdRef().backRef().key() << ", " << jet.genJetFwdRef().ref().key() << ", " << jet.genJetFwdRef().key()
	//		    << "\n";
	//}
      }
      JetAk04GenJet_->push_back(genJetIdx);
    }
  }
}

void Tupel::processPhotons(){
  for (unsigned j = 0; j < photons->size(); ++j){
    const pat::Photon& photon = (*photons)[j];
    //photon momentum
    PhotPt_->push_back(photon.pt());
    PhotEta_->push_back(photon.eta());
    PhotPhi_->push_back(photon.phi());
    PhotScRawE_->push_back(photon.superCluster()->rawEnergy());
    PhotScEta_->push_back(photon.superCluster()->eta());
    PhotScPhi_->push_back(photon.superCluster()->phi());
    
    //photon isolation
    PhotIsoEcal_->push_back(photon.ecalIso());
    PhotIsoHcal_->push_back(photon.hcalIso());
    PhotIsoTk_->push_back(photon.trackIso());
    PhotPfIsoChHad_->push_back(photon.chargedHadronIso());
    PhotPfIsoNeutralHad_->push_back(photon.neutralHadronIso());
    PhotPfIsoPhot_->push_back(photon.photonIso());
    PhotPfIsoPuChHad_->push_back(photon.puChargedHadronIso());
    PhotPfIsoEcalClus_->push_back(photon.ecalPFClusterIso());
    PhotPfIsoHcalClus_->push_back(photon.hcalPFClusterIso());
      
    //photon cluster shape
    PhotE3x3_->push_back(photon.e3x3());
    PhotE1x5_->push_back(photon.e1x5());
    //PhotE1x3_->push_back(...);
    //PhotE2x2_->push_back(...);
    PhotE2x5_->push_back(photon.e2x5());
    PhotE5x5_->push_back(photon.e5x5());
    PhotSigmaIetaIeta_->push_back(photon.sigmaIetaIeta());
    //PhotSigmaIetaIphi_->push_back(...);
    //PhotSigmaIphiIphi_->push_back(...);
    PhotEtaWidth_->push_back(photon.sigmaEtaEta());
    //PhotPhiWidth_->push_back(...);

    //preshower
    //PhotEsE_->push_back(...);
    //PhotEsSigmaIxIx_->push_back(...);
    //PhotEsSigmaIyIy_->push_back(...);
    //PhotEsSigmaIrIr_->push_back(sqrt(std::pow(PhotEsSigmaIxIx_, 2)
    //                                  + std::pow(PhotEsSigmaIyIy_, 2));

    //photon time
    //PhotTime_->push_back(...);
    
    //photon ids:
    std::vector<std::pair<std::string,Bool_t> > idlist = photon.photonIDs();
    if(!photonIdsListed_) {
      std::ofstream f("electron_id_list.txt");
      f << "Autogenerated file\n\n"
	"Supported photon ids:\n"; 
      for (unsigned k  = 0 ; k < idlist.size(); ++k){
	f << idlist[k].first << ": " << (idlist[k].second ? "yes" : "no") << "\n";
      }
      f.close();
      photonIdsListed_ = true;
    }

    int photonId = 0;
    if(photon.photonID(std::string("PhotonCutBasedIDLoose"))) photonId |= 1;
    if(photon.photonID(std::string("PhotonCutBasedIDTight"))) photonId |= 4;
    PhotId_->push_back(photonId);
    PhotHoE_->push_back(photon.hadronicOverEm()); 
    PhotHasPixelSeed_->push_back(photon.hasPixelSeed());
  }
}

void Tupel::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm; //ADD
  ++ccnevent;

  readEvent(iEvent);

  // PAT trigger EvtNum
  //edm::Handle<pat::TriggerEvent>  triggerEvent;
  //iEvent.getByLabel( triggerEvent_, triggerEvent );
  

  // edm::Handle<edm::ValueMap<float> > full5x5sieie;
  // iEvent.getByToken(full5x5SigmaIEtaIEtaMapToken_,full5x5sieie);
  
  processMET(iEvent);

  processVtx();

  if (!*EvtIsRealData_){
    processPu(iEvent);
    if (genParticles) processGenParticles(iEvent);
    processGenJets(iEvent);
    processPdfInfo(iEvent);
  }

  processTrigger(iEvent);

  if(muon) processMuons();

  //electrons B.B.
  if(electron) processElectrons();
  
  //jets
  if(jettt) processJets();

  //photons. Ph. G.
  if(photons) processPhotons();

  //Stores the EvtNum in the output tree
  treeHelper_->fill();
  
}

void
Tupel::writeHeader(){
  TTree* t = new TTree("Header", "Header");
  TString checksum_(checksum);
  t->Branch("Tupel_cc_githash", &checksum_);

  TString cmssw_release(edm::getReleaseVersion().c_str());
  t->Branch("CMSSW_RELEASE", &cmssw_release);

  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  hostname[sizeof(hostname)-1] = 0;
  TString hostname_(hostname);
  t->Branch("Hostname", &hostname_);
  
  edm::TimeOfDay ts;
  stringstream s;
  s << setprecision(0) << ts;
  TString ts_(s.str().c_str());
  t->Branch("CreationTime", &ts_);

  t->Fill();
}

void 
Tupel::beginJob()
{
  //  jecUnc = new JetCorrectionUncertainty("Fall12_V7_DATA_Uncertainty_AK5PFchs.txt");
  // register to the TFileService
  edm::Service<TFileService> fs;
  TFileDirectory TestDir = fs->mkdir("test");

  writeHeader();

  myTree = new TTree("EventTree"," EventTree");
  treeHelper_ = std::auto_ptr<TreeHelper>
    (new TreeHelper(myTree, new TTree("Description", "Description"),
		    new TTree("BitFields", "BitFields")));

  //Event
  ADD_BRANCH_D(EvtIsRealData, "True if real data, false if MC");
  ADD_BRANCH_D(EvtNum, "Event number");
  ADD_BRANCH_D(EvtRunNum, "Run number");
  ADD_BRANCH_D(EvtLumiNum, "Luminosity block number");
  ADD_BRANCH_D(EvtBxNum, "Bunch crossing number");
  ADD_BRANCH_D(EvtVtxCnt, "Number of reconstructed primary vertices");
  ADD_BRANCH_D(EvtPuCnt, "Number of measured pile-up events");
  ADD_BRANCH_D(EvtPuCntTruth, "True number of pile-up events");
  ADD_BRANCH(EvtWeights); //description filled in endRun()
  ADD_BRANCH_D(EvtFastJetRho, "Fastjet pile-up variable \\rho");
  
  //Trigger
  ADD_BRANCH_D(TrigHlt, "HLT triggger bits. See BitField.TrigHlt for bit description.");
  ADD_BRANCH_D(TrigHltPhot, "HLT Photon triggger bits. See BitField.TrigHltPhot for bit description.");
  ADD_BRANCH_D(TrigHltMu, "HLT Muon triggger bits. See BitField.TrigHltMu for bit description.");
  ADD_BRANCH_D(TrigHltDiMu, "HLT Dimuon triggger bits. See BitField.TrigHltDiMu for bit description.");
  
  //Missing Energy
  treeHelper_->addDescription("MET", "PF MET");
  ADD_BRANCH(METPt);
  ADD_BRANCH(METPx);
  ADD_BRANCH(METPy);
  ADD_BRANCH(METPz);
  ADD_BRANCH(METE);
  ADD_BRANCH(METsigx2);
  ADD_BRANCH(METsigxy);
  ADD_BRANCH(METsigy2);
  ADD_BRANCH(METsig);

  //Generator level leptons.
  treeHelper_->addDescription("GLepDr01", "Generator-level leptons. Muons and electrons and their antiparticles are dressed using a cone of radius R = 0.1");
  ADD_BRANCH(GLepDr01Pt);
  ADD_BRANCH(GLepDr01Eta);
  ADD_BRANCH(GLepDr01Phi);
  ADD_BRANCH(GLepDr01E);
  ADD_BRANCH(GLepDr01Id);
  ADD_BRANCH(GLepDr01St);
  ADD_BRANCH(GLepDr01MomId);
  treeHelper_->addDescription("GLepBare", "Generator-level leptons, status 1 without dressing.");
  ADD_BRANCH(GLepBarePt);
  ADD_BRANCH(GLepBareEta);
  ADD_BRANCH(GLepBarePhi);
  ADD_BRANCH(GLepBareE);
  ADD_BRANCH(GLepBareId);
  ADD_BRANCH(GLepBareSt);
  ADD_BRANCH(GLepBareMomId);
  treeHelper_->addDescription("GLepDr01", "Status 3 generator-level leptons.");
  ADD_BRANCH(GLepSt3Pt);
  ADD_BRANCH(GLepSt3Eta);
  ADD_BRANCH(GLepSt3Phi);
  ADD_BRANCH(GLepSt3E);
  ADD_BRANCH(GLepSt3Id);
  ADD_BRANCH(GLepSt3St);
  ADD_BRANCH_D(GLepSt3Mother0Id, "Lepton mother PDG Id. Filled only for first mother. Number of mothers can be checked in GLepoSt3MotherCnt.");
  ADD_BRANCH(GLepSt3MotherCnt);

  //Photons in the vicinity of the leptons
  treeHelper_->addDescription("GLepClosePhot", "Photons aroud leptons. Selection cone size: R = 0.2");
  ADD_BRANCH(GLepClosePhotPt);
  ADD_BRANCH(GLepClosePhotEta);
  ADD_BRANCH(GLepClosePhotPhi);
  ADD_BRANCH(GLepClosePhotE);
  ADD_BRANCH(GLepClosePhotId);
  ADD_BRANCH_D(GLepClosePhotMother0Id, "Photon mother PDG Id. Filled only for first mother. Number of mothers can be checked in GLepoSt3MotherCnt.");
  ADD_BRANCH(GLepClosePhotMotherCnt);
  ADD_BRANCH(GLepClosePhotSt);

  //Gen Jets
  treeHelper_->addDescription("GJetAk04", "Generator-level reconstructed with the anti-kt algorithm with distance parameter R = 0.4");
  ADD_BRANCH(GJetAk04Pt);
  ADD_BRANCH(GJetAk04Eta);
  ADD_BRANCH(GJetAk04Phi);
  ADD_BRANCH(GJetAk04E);
  ADD_BRANCH(GJetAk04ChFrac);
  ADD_BRANCH(GJetAk04ConstCnt);
  ADD_BRANCH(GJetAk04ConstId);
  ADD_BRANCH(GJetAk04ConstPt);
  ADD_BRANCH(GJetAk04ConstEta);
  ADD_BRANCH(GJetAk04ConstPhi);
  ADD_BRANCH(GJetAk04ConstE);

  //Exta generator information
  ADD_BRANCH_D(GPdfId1, "PDF Id for beam 1");
  ADD_BRANCH_D(GPdfId2, "PDF Id for beam 2");
  ADD_BRANCH_D(GPdfx1, "PDF x for beam 1");
  ADD_BRANCH_D(GPdfx2, "PDF x for beam 1");
  ADD_BRANCH_D(GPdfScale, "PDF energy scale");
  ADD_BRANCH_D(GBinningValue, "Value of the observable used to split the MC sample generation (e.g. pt_hat for a pt_hat binned MC sample).");
  ADD_BRANCH_D(GNup, "Number of particles/partons included in the matrix element.");   
  
  //Muons
  treeHelper_->addDescription("Mu", "PF reconstruced muons.");
  ADD_BRANCH(MuPt);
  ADD_BRANCH(MuEta);
  ADD_BRANCH(MuPhi);
  ADD_BRANCH(MuE);
  ADD_BRANCH(MuId);
  ADD_BRANCH_D(MuIdTight, "Muon tight id. Bit field, one bit per primary vertex hypothesis. Bit position corresponds to index in EvtVtx");
  ADD_BRANCH(MuCh);  
  ADD_BRANCH(MuVtxZ);
  ADD_BRANCH(MuDxy);
  ADD_BRANCH(MuIsoRho);
  ADD_BRANCH(MuPfIso);
  ADD_BRANCH(MuType);
  ADD_BRANCH(MuIsoTkIsoAbs);
  ADD_BRANCH(MuIsoTkIsoRel);
  ADD_BRANCH(MuIsoCalAbs);
  ADD_BRANCH(MuIsoCombRel);
  ADD_BRANCH(MuTkNormChi2);
  ADD_BRANCH(MuTkHitCnt);
  ADD_BRANCH(MuMatchedStationCnt);
  ADD_BRANCH(MuDz);
  ADD_BRANCH(MuPixelHitCnt);
  ADD_BRANCH(MuTkLayerCnt);
  ADD_BRANCH(MuPfIsoChHad);
  ADD_BRANCH(MuPfIsoNeutralHad);
  ADD_BRANCH(MuPfIsoRawRel);
  ADD_BRANCH(MuHltMatch);

  //Electrons
  treeHelper_->addDescription("El", "PF reconstructed electrons");
  ADD_BRANCH(ElPt);
  ADD_BRANCH(ElEta);
  ADD_BRANCH(ElEtaSc);
  ADD_BRANCH(ElPhi);
  ADD_BRANCH(ElE);
  ADD_BRANCH(ElId);
  ADD_BRANCH(ElCh);
  ADD_BRANCH(ElMvaTrig);
  ADD_BRANCH(ElMvaNonTrig);
  ADD_BRANCH(ElMvaPresel);
  ADD_BRANCH(ElDEtaTkScAtVtx);
  ADD_BRANCH(ElDPhiTkScAtVtx);
  ADD_BRANCH(ElHoE);
  ADD_BRANCH(ElSigmaIetaIeta);
  ADD_BRANCH(ElSigmaIetaIetaFull5x5);
  ADD_BRANCH(ElEinvMinusPinv);
  ADD_BRANCH(ElD0);
  ADD_BRANCH(ElDz);
  ADD_BRANCH(ElExpectedMissingInnerHitCnt);
  ADD_BRANCH(ElPassConvVeto);
  ADD_BRANCH(ElHltMatch);
  ADD_BRANCH(ElPfIsoChHad);
  ADD_BRANCH(ElPfIsoNeutralHad);
  ADD_BRANCH(ElPfIsoIso);
  ADD_BRANCH(ElPfIsoPuChHad);
  ADD_BRANCH(ElPfIsoRaw);
  ADD_BRANCH(ElPfIsoDbeta);
  ADD_BRANCH(ElPfIsoRho);
  ADD_BRANCH_D(ElAEff, "Electron effecive area");

  ADD_BRANCH(charged);
  ADD_BRANCH(photon);
  ADD_BRANCH(neutral);
  ADD_BRANCH(charged_Tom);
  ADD_BRANCH(photon_Tom);
  ADD_BRANCH(neutral_Tom);


  //photon momenta
  treeHelper_->addDescription("Phot", "Particle flow photons");
  ADD_BRANCH(PhotPt);;
  ADD_BRANCH(PhotEta);;
  ADD_BRANCH(PhotPhi);;
  ADD_BRANCH_D(PhotScRawE, "Photon Supercluster uncorrected energy");
  ADD_BRANCH_D(PhotScEta, "Photon Supercluster eta");
  ADD_BRANCH_D(PhotScPhi, "Photon Supercluster phi");

  //photon isolations
  ADD_BRANCH(PhotIsoEcal);
  ADD_BRANCH(PhotIsoHcal);
  ADD_BRANCH(PhotIsoTk);
  ADD_BRANCH(PhotPfIsoChHad);
  ADD_BRANCH(PhotPfIsoNeutralHad);
  ADD_BRANCH(PhotPfIsoPhot);
  ADD_BRANCH(PhotPfIsoPuChHad);
  ADD_BRANCH(PhotPfIsoEcalClus);
  ADD_BRANCH(PhotPfIsoHcalClus);
  
  //photon cluster shapes
  ADD_BRANCH_D(PhotE3x3, "Photon energy deposited in 3x3 ECAL crystal array. Divide this quantity by PhotScRawE to obtain the R9 variable.");
  ADD_BRANCH(PhotE1x5);
  //ADD_BRANCH(PhotE1x3);
  //ADD_BRANCH(PhotE2x2);
  ADD_BRANCH(PhotE2x5);
  ADD_BRANCH(PhotE5x5);
  ADD_BRANCH(PhotSigmaIetaIeta);
  //ADD_BRANCH(PhotSigmaIetaIphi);
  //ADD_BRANCH(PhotSigmaIphiIphi);
  ADD_BRANCH(PhotEtaWidth);
  ADD_BRANCH(PhotPhiWidth);
  ADD_BRANCH(PhotHoE);

  //photon ES
  //ADD_BRANCH_D(PhotEsE, "Photon. Energy deposited in the preshower");
  //ADD_BRANCH_D(PhotEsSigmaIxIx, "Photon. Preshower cluster extend along x-axis");
  //ADD_BRANCH_D(PhotEsSigmaIyIy, "Photon. Preshower cluster extend along y-axis");
  //ADD_BRANCH_D(PhotEsSigmaIrIr, "Photon. \\sqrt(PhotEsSigmaIxIx**2+PhotEsSigmaIyIy**2)");
  
  //photon ID
  ADD_BRANCH_D(PhotId, "Photon Id. Field of bits described in BitFields.PhotId");
  ADD_BRANCH_D(PhotHasPixelSeed, "Pixel and tracker based variable to discreminate photons from electrons");

  //photon timing
  //ADD_BRANCH_D(PhotTime, "Photon. Timing from ECAL");
  
  //PF Jets
  treeHelper_->addDescription("JetAk04", "Reconstricuted jets clustered with the anti-ket algorithm with distance parameter R = 0.4");
  ADD_BRANCH(JetAk04Pt);
  ADD_BRANCH(JetAk04Eta);
  ADD_BRANCH(JetAk04Phi);
  ADD_BRANCH(JetAk04E);
  ADD_BRANCH_D(JetAk04Id, "Id to reject fake jets from electronic noice");
  ADD_BRANCH_D(JetAk04PuId, "Id to reject jets from pile-up events");
  ADD_BRANCH_D(JetAk04PuMva, "MVA based descriminant for PU jets");
  ADD_BRANCH_D(JetAk04RawPt, "Jet Pt before corrections");
  ADD_BRANCH_D(JetAk04RawE, "Jet energy before corrections");
  ADD_BRANCH_D(JetAk04HfHadE, "Jet hadronic energy deposited in HF");
  ADD_BRANCH_D(JetAk04HfEmE, "Jet electromagnetic energy deposited in HF");
  ADD_BRANCH(JetAk04ChHadFrac);
  ADD_BRANCH(JetAk04NeutralHadAndHfFrac);
  ADD_BRANCH(JetAk04ChEmFrac);
  ADD_BRANCH(JetAk04NeutralEmFrac);
  ADD_BRANCH(JetAk04ChMult);
  ADD_BRANCH(JetAk04ConstCnt);
  ADD_BRANCH(JetAk04JetBeta);
  ADD_BRANCH(JetAk04JetBetaClassic);
  ADD_BRANCH(JetAk04JetBetaStar);
  ADD_BRANCH(JetAk04JetBetaStarClassic);
  treeHelper_->addDescription("JetAk04BTag", "B tagging with different algorithms");
  ADD_BRANCH_D(JetAk04BTagCsv, "combinedSecondaryVertexBJetTags");
  ADD_BRANCH_D(JetAk04BTagCsvV1, "combinedSecondaryVertexV1BJetTags");
  ADD_BRANCH_D(JetAk04BTagCsvSLV1, "combinedSecondaryVertexSoftPFLeptonV1BJetTags");
  ADD_BRANCH_D(JetAk04BDiscCisvV2, "pfCombinedInclusiveSecondaryVertexV2BJetTags");
  ADD_BRANCH_D(JetAk04BDiscJp, "pfJetProbabilityBJetTags");
  ADD_BRANCH_D(JetAk04BDiscBjp, "pfJetBProbabilityBJetTags");
  ADD_BRANCH_D(JetAk04BDiscTche, "pfTrackCountingHighEffBJetTags");
  ADD_BRANCH_D(JetAk04BDiscTchp, "pfTrackCountingHighPurBJetTags");
  ADD_BRANCH_D(JetAk04BDiscSsvhe, "pfSimpleSecondaryVertexHighEffBJetTags");
  ADD_BRANCH_D(JetAk04BDiscSsvhp, "pfSimpleSecondaryVertexHighPurBJetTags");
  ADD_BRANCH_D(JetAk04PartFlav, "Quark-based jet.");
  ADD_BRANCH(JetAk04JecUncUp);
  ADD_BRANCH(JetAk04JecUncDwn);
  ADD_BRANCH(JetAk04ConstId);
  ADD_BRANCH(JetAk04ConstPt);
  ADD_BRANCH(JetAk04ConstEta);
  ADD_BRANCH(JetAk04ConstPhi);
  ADD_BRANCH(JetAk04ConstE);
  ADD_BRANCH(JetAk04GenJet);

  defineBitFields();

}

void 
Tupel::endJob() 
{
  //  delete jecUnc;
  //  myTree->Print();
  treeHelper_->fillDescriptionTree();
}

void
Tupel::endRun(edm::Run const& iRun, edm::EventSetup const&){

  std::string desc = "List of MC event weights. The first weight is the default weight to use when filling histograms.";
  edm::Handle<LHERunInfoProduct> lheRun;  
  iRun.getByLabel(lheSource_, lheRun );
 
  if(!lheRun.failedToGet ()){
    const LHERunInfoProduct& myLHERunInfoProduct = *(lheRun.product());   
    for (std::vector<LHERunInfoProduct::Header>::const_iterator iter = myLHERunInfoProduct.headers_begin();
	 iter != myLHERunInfoProduct.headers_end();
	 iter++){
      if(iter->tag() == "initrwgt" && iter->size() > 0){
	desc += "\n";
	for(std::vector<std::string>::const_iterator it = iter->begin();
	    it != iter->end(); ++it){
	  desc += *it;
	}
	break;
      }
    }
  }
  //Suppresses spurious last line with "<" produced with CMSSW_7_4_1:
  if(desc.size() > 2 && desc[desc.size()-1] == '<'){
    std::string::size_type p = desc.find_last_not_of(" \n", desc.size()-2);
    if(p != std::string::npos) desc.erase(p + 1);
  }
  treeHelper_->addDescription("EvtWeights", desc.c_str());
}

DEFINE_FWK_MODULE(Tupel);
