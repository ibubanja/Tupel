import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('python')
options.register('runOnData', False,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.bool,
                 "Run this on real data"
                 )
options.parseArguments()
process = cms.Process("S2")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
from JetMETCorrections.Configuration.DefaultJEC_cff import *
from JetMETCorrections.Configuration.JetCorrectionServices_cff import *

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_Prompt_ICHEP16JEC_v0' if options.runOnData else '80X_mcRun2_asymptotic_2016_miniAODv2_v1')
#dataFile='/store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext3-v1/00000/0064B539-803A-E611-BDEA-002590D0B060.root'
dataFile='file:pickevents.root'
#dataFile='file:AToTT_MiniAOD_100.root'
jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute']
jecFile='sqlite:Spring16_25nsV6_MC.db'
jecTag='JetCorrectorParametersCollection_Spring16_25nsV6_MC_AK4PFchs'
if options.runOnData :
	jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']
	jecFile='sqlite:Spring16_25nsV6_DATA.db'
	jecTag='JetCorrectorParametersCollection_Spring16_25nsV6_DATA_AK4PFchs'	
	dataFile='/store/data/Run2016D/SingleElectron/MINIAOD/PromptReco-v2/000/276/315/00000/10BB1858-0045-E611-83A5-02163E01456D.root'
#	dataFile='/store/data/Run2016D/SingleMuon/MINIAOD/PromptReco-v2/000/276/315/00000/168C3DE5-F444-E611-A012-02163E014230.root'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(dataFile)

)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
updateJetCollection(
   process,
   jetSource = cms.InputTag('slimmedJets'),
   labelName = 'UpdatedJEC',
   jetCorrections = ('AK4PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
)

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
runMetCorAndUncFromMiniAOD(process,
                           isData=options.runOnData,
                           metType = 'PF',
                           postfix=''
                           )

process.load('Configuration.StandardSequences.Services_cff')
process.load("JetMETCorrections.Modules.JetResolutionESProducer_cfi")

process.load("CondCore.CondDB.CondDB_cfi")
import os

process.jer = cms.ESSource("PoolDBESSource",
	DBParameters = cms.PSet(
	messageLevel = cms.untracked.int32(0)
	),
	toGet = cms.VPSet(
		cms.PSet(
		record = cms.string('JetCorrectionsRecord'),
		tag    = cms.string(jecTag),
		label  = cms.untracked.string('AK4PFchs')
		),
	),
        connect = cms.string(jecFile)
        )

process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'jer')

if not options.runOnData :
	process.pseudoTop = cms.EDProducer("PseudoTopProducer",
    	finalStates = cms.InputTag("packedGenParticles"),
    	genParticles = cms.InputTag("prunedGenParticles"),
    	jetConeSize = cms.double(0.4),
    	maxJetEta = cms.double(2.4),
    	minJetPt = cms.double(20),
    	leptonConeSize = cms.double(0.1),
	minLeptonPt = cms.double(20),#new?
     	maxLeptonEta= cms.double(2.4),#new?
     	minLeptonPtDilepton = cms.double(20),#new?
     	maxLeptonEtaDilepton= cms.double(2.4),#new?
	minDileptonMassDilepton= cms.double(0.),#new?
     	minLeptonPtSemilepton = cms.double(20),#new?
     	maxLeptonEtaSemilepton= cms.double(2.4),#new?
     	minVetoLeptonPtSemilepton= cms.double(15.),#new?
     	maxVetoLeptonEtaSemilepton= cms.double(2.4),#new?
     	minMETSemiLepton=cms.double(0.),#new?
     	minMtWSemiLepton=cms.double(0.),#new?
    	tMass = cms.double(172.5),
    	wMass = cms.double(80.4)
	)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('ntuple.root' )
)

jetsrcc="updatedPatJetsUpdatedJEC"
trigPath="HLT2"
trigFiltPath="PAT"
if options.runOnData :
	trigFiltPath="RECO"
	trigPath="HLT"
process.tupel = cms.EDAnalyzer("Tupel",
  triggerfilters = cms.InputTag("TriggerResults","",trigFiltPath),
  triggerEvent   = cms.InputTag("patTriggerEvent" ),
  HLTSrc         = cms.InputTag("TriggerResults","",trigPath), 
  photonSrc      = cms.InputTag("slimmedPhotons"),
  electronSrc    = cms.InputTag("slimmedElectrons"),
  muonSrc        = cms.InputTag("slimmedMuons"),
  #tauSrc        = cms.untracked.InputTag("slimmedPatTaus"),
  pfcandSrc	 = cms.InputTag("packedPFCandidates"),
  jetSrc         = cms.InputTag(jetsrcc),
  metSrc         = cms.InputTag("patMETsPF"),
  genSrc         = cms.InputTag("prunedGenParticles"),
  pgenSrc        =cms.InputTag("packedGenParticles"),
  gjetSrc        = cms.InputTag('slimmedGenJets'),
  muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
  muonMatch2    = cms.string( 'muonTriggerMatchHLTMuons2' ),
  elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
  channel    = cms.string( 'noselection' ),
  keepparticlecoll    = cms.bool(False),
  mSrcRho      = cms.InputTag('fixedGridRhoFastjetAll'),#arbitrary rho now
  CalojetLabel = cms.InputTag('slimmedJets'), #same collection now BB 
#  jecunctable = cms.string(jecunctable_),
  #metSource = cms.VInputTag("slimmedMETs","slimmedMETs","slimmedMETs","slimmedMETs"), #no MET corr yet
  metSource = cms.VInputTag("slimmedMETs","slimmedMETs"),
  lheSource=cms.InputTag("source")
)

from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
#process.goodOfflinePrimaryVertices = cms.EDFilter(
#    "PrimaryVertexObjectFilter",
#    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0),maxd0 = cms.double(2.0) ),
#    src=cms.InputTag('offlineSlimmedPrimaryVertices')
#    )

#process.goodOfflinePrimaryVertices = cms.EDFilter(
#    "PrimaryVertexObjectFilter",
#    filterParams = pvSelector.clone( NPV     = cms.int32(1),minNdof = cms.double(4.0), maxZ = cms.double(24.0),maxRho = cms.double(2.0) ),
#    src=cms.InputTag('offlineSlimmedPrimaryVertices')
#    )

process.p = cms.Path(
#    process.patJets
# + process.patMETs
# + process.inclusiveSecondaryVertexFinderTagInfos
#+process.selectedMuons
#+process.selectedElectrons 
# +process.goodOfflinePrimaryVertices 
#+process.kt6PFJets
#process.pseudoTop
#+process.JetResolutionESProducer_AK4PFchs
#+process.JetResolutionESProducer_SF_AK4PFchs 
 process.tupel 
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)


process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('test.root'),
#    outputCommands = cms.untracked.vstring(['drop *','keep patJets_patJets_*_*','keep *_*_*_PAT','keep recoTracks_unp*_*_*','keep recoVertexs_unp*_*_*'])
#    outputCommands = cms.untracked.vstring(['drop *'])
outputCommands = cms.untracked.vstring(['drop *', 'keep *_slimmedMETs*_*_*'])
)
#process.endpath= cms.EndPath(process.out)


#from PhysicsTools.PatAlgos.tools.trigTools import *
#switchOnTrigger( process ) # This is optional and can be omitted.

# Switch to selected PAT objects in the trigger matching
#removeCleaningFromTriggerMatching( process )
##############################
iFileName = "fileNameDump_cfg.py"
file = open(iFileName,'w')
file.write(str(process.dumpPython()))
file.close()


