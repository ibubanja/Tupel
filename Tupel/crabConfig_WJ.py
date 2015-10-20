from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'testwj_20151009_new'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'simple_run_data_cfg.py'
#config.JobType.scriptExe = 'job_crab.sh'
#config.JobType.outputFiles=['events_presys.lhe']
#config.JobType.inputFiles = ['FrameworkJobReport.xml']

config.section_("Data")
#config.Data.inputDataset = '/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v1/MINIAODSIM'
config.Data.inputDataset = '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
#config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 1000
config.Data.totalUnits= -1
#config.Data.outLFN = '/store/caf/user/efe/ntupel/t012j_5f_ckm_NLO_FXFX/crab_seed_4500_' # or '/store/group/<subdir>'
config.Data.outLFNDirBase = '/store/group/phys_top/bbilin/n-tupel/25ns_pf_reduced'
config.Data.publication = False
config.Data.publishDataName = 'WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1'
#config.Data.primaryDataset='CRAB_PrivateMC'
config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.whitelist = ["T2_US_Nebraska"]

