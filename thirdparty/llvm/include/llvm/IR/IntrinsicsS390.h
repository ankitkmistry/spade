/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Intrinsic Function Source Fragment                                         *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM_IR_INTRINSIC_S390_ENUMS_H
#define LLVM_IR_INTRINSIC_S390_ENUMS_H
namespace llvm::Intrinsic {
enum S390Intrinsics : unsigned {
// Enum values for intrinsics.
    s390_bdepg = 11640,                                // llvm.s390.bdepg
    s390_bextg,                                // llvm.s390.bextg
    s390_efpc,                                 // llvm.s390.efpc
    s390_etnd,                                 // llvm.s390.etnd
    s390_lcbb,                                 // llvm.s390.lcbb
    s390_ntstg,                                // llvm.s390.ntstg
    s390_ppa_txassist,                         // llvm.s390.ppa.txassist
    s390_sfpc,                                 // llvm.s390.sfpc
    s390_tabort,                               // llvm.s390.tabort
    s390_tbegin,                               // llvm.s390.tbegin
    s390_tbegin_nofloat,                       // llvm.s390.tbegin.nofloat
    s390_tbeginc,                              // llvm.s390.tbeginc
    s390_tdc,                                  // llvm.s390.tdc
    s390_tend,                                 // llvm.s390.tend
    s390_vaccb,                                // llvm.s390.vaccb
    s390_vacccq,                               // llvm.s390.vacccq
    s390_vaccf,                                // llvm.s390.vaccf
    s390_vaccg,                                // llvm.s390.vaccg
    s390_vacch,                                // llvm.s390.vacch
    s390_vaccq,                                // llvm.s390.vaccq
    s390_vacq,                                 // llvm.s390.vacq
    s390_vaq,                                  // llvm.s390.vaq
    s390_vavgb,                                // llvm.s390.vavgb
    s390_vavgf,                                // llvm.s390.vavgf
    s390_vavgg,                                // llvm.s390.vavgg
    s390_vavgh,                                // llvm.s390.vavgh
    s390_vavglb,                               // llvm.s390.vavglb
    s390_vavglf,                               // llvm.s390.vavglf
    s390_vavglg,                               // llvm.s390.vavglg
    s390_vavglh,                               // llvm.s390.vavglh
    s390_vavglq,                               // llvm.s390.vavglq
    s390_vavgq,                                // llvm.s390.vavgq
    s390_vbperm,                               // llvm.s390.vbperm
    s390_vceqbs,                               // llvm.s390.vceqbs
    s390_vceqfs,                               // llvm.s390.vceqfs
    s390_vceqgs,                               // llvm.s390.vceqgs
    s390_vceqhs,                               // llvm.s390.vceqhs
    s390_vceqqs,                               // llvm.s390.vceqqs
    s390_vcfn,                                 // llvm.s390.vcfn
    s390_vchbs,                                // llvm.s390.vchbs
    s390_vchfs,                                // llvm.s390.vchfs
    s390_vchgs,                                // llvm.s390.vchgs
    s390_vchhs,                                // llvm.s390.vchhs
    s390_vchlbs,                               // llvm.s390.vchlbs
    s390_vchlfs,                               // llvm.s390.vchlfs
    s390_vchlgs,                               // llvm.s390.vchlgs
    s390_vchlhs,                               // llvm.s390.vchlhs
    s390_vchlqs,                               // llvm.s390.vchlqs
    s390_vchqs,                                // llvm.s390.vchqs
    s390_vcksm,                                // llvm.s390.vcksm
    s390_vclfnhs,                              // llvm.s390.vclfnhs
    s390_vclfnls,                              // llvm.s390.vclfnls
    s390_vcnf,                                 // llvm.s390.vcnf
    s390_vcrnfs,                               // llvm.s390.vcrnfs
    s390_verimb,                               // llvm.s390.verimb
    s390_verimf,                               // llvm.s390.verimf
    s390_verimg,                               // llvm.s390.verimg
    s390_verimh,                               // llvm.s390.verimh
    s390_veval,                                // llvm.s390.veval
    s390_vfaeb,                                // llvm.s390.vfaeb
    s390_vfaebs,                               // llvm.s390.vfaebs
    s390_vfaef,                                // llvm.s390.vfaef
    s390_vfaefs,                               // llvm.s390.vfaefs
    s390_vfaeh,                                // llvm.s390.vfaeh
    s390_vfaehs,                               // llvm.s390.vfaehs
    s390_vfaezb,                               // llvm.s390.vfaezb
    s390_vfaezbs,                              // llvm.s390.vfaezbs
    s390_vfaezf,                               // llvm.s390.vfaezf
    s390_vfaezfs,                              // llvm.s390.vfaezfs
    s390_vfaezh,                               // llvm.s390.vfaezh
    s390_vfaezhs,                              // llvm.s390.vfaezhs
    s390_vfcedbs,                              // llvm.s390.vfcedbs
    s390_vfcesbs,                              // llvm.s390.vfcesbs
    s390_vfchdbs,                              // llvm.s390.vfchdbs
    s390_vfchedbs,                             // llvm.s390.vfchedbs
    s390_vfchesbs,                             // llvm.s390.vfchesbs
    s390_vfchsbs,                              // llvm.s390.vfchsbs
    s390_vfeeb,                                // llvm.s390.vfeeb
    s390_vfeebs,                               // llvm.s390.vfeebs
    s390_vfeef,                                // llvm.s390.vfeef
    s390_vfeefs,                               // llvm.s390.vfeefs
    s390_vfeeh,                                // llvm.s390.vfeeh
    s390_vfeehs,                               // llvm.s390.vfeehs
    s390_vfeezb,                               // llvm.s390.vfeezb
    s390_vfeezbs,                              // llvm.s390.vfeezbs
    s390_vfeezf,                               // llvm.s390.vfeezf
    s390_vfeezfs,                              // llvm.s390.vfeezfs
    s390_vfeezh,                               // llvm.s390.vfeezh
    s390_vfeezhs,                              // llvm.s390.vfeezhs
    s390_vfeneb,                               // llvm.s390.vfeneb
    s390_vfenebs,                              // llvm.s390.vfenebs
    s390_vfenef,                               // llvm.s390.vfenef
    s390_vfenefs,                              // llvm.s390.vfenefs
    s390_vfeneh,                               // llvm.s390.vfeneh
    s390_vfenehs,                              // llvm.s390.vfenehs
    s390_vfenezb,                              // llvm.s390.vfenezb
    s390_vfenezbs,                             // llvm.s390.vfenezbs
    s390_vfenezf,                              // llvm.s390.vfenezf
    s390_vfenezfs,                             // llvm.s390.vfenezfs
    s390_vfenezh,                              // llvm.s390.vfenezh
    s390_vfenezhs,                             // llvm.s390.vfenezhs
    s390_vfidb,                                // llvm.s390.vfidb
    s390_vfisb,                                // llvm.s390.vfisb
    s390_vfmaxdb,                              // llvm.s390.vfmaxdb
    s390_vfmaxsb,                              // llvm.s390.vfmaxsb
    s390_vfmindb,                              // llvm.s390.vfmindb
    s390_vfminsb,                              // llvm.s390.vfminsb
    s390_vftcidb,                              // llvm.s390.vftcidb
    s390_vftcisb,                              // llvm.s390.vftcisb
    s390_vgemb,                                // llvm.s390.vgemb
    s390_vgemf,                                // llvm.s390.vgemf
    s390_vgemg,                                // llvm.s390.vgemg
    s390_vgemh,                                // llvm.s390.vgemh
    s390_vgemq,                                // llvm.s390.vgemq
    s390_vgfmab,                               // llvm.s390.vgfmab
    s390_vgfmaf,                               // llvm.s390.vgfmaf
    s390_vgfmag,                               // llvm.s390.vgfmag
    s390_vgfmah,                               // llvm.s390.vgfmah
    s390_vgfmb,                                // llvm.s390.vgfmb
    s390_vgfmf,                                // llvm.s390.vgfmf
    s390_vgfmg,                                // llvm.s390.vgfmg
    s390_vgfmh,                                // llvm.s390.vgfmh
    s390_vistrb,                               // llvm.s390.vistrb
    s390_vistrbs,                              // llvm.s390.vistrbs
    s390_vistrf,                               // llvm.s390.vistrf
    s390_vistrfs,                              // llvm.s390.vistrfs
    s390_vistrh,                               // llvm.s390.vistrh
    s390_vistrhs,                              // llvm.s390.vistrhs
    s390_vlbb,                                 // llvm.s390.vlbb
    s390_vll,                                  // llvm.s390.vll
    s390_vlrl,                                 // llvm.s390.vlrl
    s390_vmaeb,                                // llvm.s390.vmaeb
    s390_vmaef,                                // llvm.s390.vmaef
    s390_vmaeg,                                // llvm.s390.vmaeg
    s390_vmaeh,                                // llvm.s390.vmaeh
    s390_vmahb,                                // llvm.s390.vmahb
    s390_vmahf,                                // llvm.s390.vmahf
    s390_vmahg,                                // llvm.s390.vmahg
    s390_vmahh,                                // llvm.s390.vmahh
    s390_vmahq,                                // llvm.s390.vmahq
    s390_vmaleb,                               // llvm.s390.vmaleb
    s390_vmalef,                               // llvm.s390.vmalef
    s390_vmaleg,                               // llvm.s390.vmaleg
    s390_vmaleh,                               // llvm.s390.vmaleh
    s390_vmalhb,                               // llvm.s390.vmalhb
    s390_vmalhf,                               // llvm.s390.vmalhf
    s390_vmalhg,                               // llvm.s390.vmalhg
    s390_vmalhh,                               // llvm.s390.vmalhh
    s390_vmalhq,                               // llvm.s390.vmalhq
    s390_vmalob,                               // llvm.s390.vmalob
    s390_vmalof,                               // llvm.s390.vmalof
    s390_vmalog,                               // llvm.s390.vmalog
    s390_vmaloh,                               // llvm.s390.vmaloh
    s390_vmaob,                                // llvm.s390.vmaob
    s390_vmaof,                                // llvm.s390.vmaof
    s390_vmaog,                                // llvm.s390.vmaog
    s390_vmaoh,                                // llvm.s390.vmaoh
    s390_vmeb,                                 // llvm.s390.vmeb
    s390_vmef,                                 // llvm.s390.vmef
    s390_vmeg,                                 // llvm.s390.vmeg
    s390_vmeh,                                 // llvm.s390.vmeh
    s390_vmhb,                                 // llvm.s390.vmhb
    s390_vmhf,                                 // llvm.s390.vmhf
    s390_vmhg,                                 // llvm.s390.vmhg
    s390_vmhh,                                 // llvm.s390.vmhh
    s390_vmhq,                                 // llvm.s390.vmhq
    s390_vmleb,                                // llvm.s390.vmleb
    s390_vmlef,                                // llvm.s390.vmlef
    s390_vmleg,                                // llvm.s390.vmleg
    s390_vmleh,                                // llvm.s390.vmleh
    s390_vmlhb,                                // llvm.s390.vmlhb
    s390_vmlhf,                                // llvm.s390.vmlhf
    s390_vmlhg,                                // llvm.s390.vmlhg
    s390_vmlhh,                                // llvm.s390.vmlhh
    s390_vmlhq,                                // llvm.s390.vmlhq
    s390_vmlob,                                // llvm.s390.vmlob
    s390_vmlof,                                // llvm.s390.vmlof
    s390_vmlog,                                // llvm.s390.vmlog
    s390_vmloh,                                // llvm.s390.vmloh
    s390_vmob,                                 // llvm.s390.vmob
    s390_vmof,                                 // llvm.s390.vmof
    s390_vmog,                                 // llvm.s390.vmog
    s390_vmoh,                                 // llvm.s390.vmoh
    s390_vmslg,                                // llvm.s390.vmslg
    s390_vpdi,                                 // llvm.s390.vpdi
    s390_vperm,                                // llvm.s390.vperm
    s390_vpklsf,                               // llvm.s390.vpklsf
    s390_vpklsfs,                              // llvm.s390.vpklsfs
    s390_vpklsg,                               // llvm.s390.vpklsg
    s390_vpklsgs,                              // llvm.s390.vpklsgs
    s390_vpklsh,                               // llvm.s390.vpklsh
    s390_vpklshs,                              // llvm.s390.vpklshs
    s390_vpksf,                                // llvm.s390.vpksf
    s390_vpksfs,                               // llvm.s390.vpksfs
    s390_vpksg,                                // llvm.s390.vpksg
    s390_vpksgs,                               // llvm.s390.vpksgs
    s390_vpksh,                                // llvm.s390.vpksh
    s390_vpkshs,                               // llvm.s390.vpkshs
    s390_vsbcbiq,                              // llvm.s390.vsbcbiq
    s390_vsbiq,                                // llvm.s390.vsbiq
    s390_vscbib,                               // llvm.s390.vscbib
    s390_vscbif,                               // llvm.s390.vscbif
    s390_vscbig,                               // llvm.s390.vscbig
    s390_vscbih,                               // llvm.s390.vscbih
    s390_vscbiq,                               // llvm.s390.vscbiq
    s390_vsl,                                  // llvm.s390.vsl
    s390_vslb,                                 // llvm.s390.vslb
    s390_vsld,                                 // llvm.s390.vsld
    s390_vsldb,                                // llvm.s390.vsldb
    s390_vsq,                                  // llvm.s390.vsq
    s390_vsra,                                 // llvm.s390.vsra
    s390_vsrab,                                // llvm.s390.vsrab
    s390_vsrd,                                 // llvm.s390.vsrd
    s390_vsrl,                                 // llvm.s390.vsrl
    s390_vsrlb,                                // llvm.s390.vsrlb
    s390_vstl,                                 // llvm.s390.vstl
    s390_vstrcb,                               // llvm.s390.vstrcb
    s390_vstrcbs,                              // llvm.s390.vstrcbs
    s390_vstrcf,                               // llvm.s390.vstrcf
    s390_vstrcfs,                              // llvm.s390.vstrcfs
    s390_vstrch,                               // llvm.s390.vstrch
    s390_vstrchs,                              // llvm.s390.vstrchs
    s390_vstrczb,                              // llvm.s390.vstrczb
    s390_vstrczbs,                             // llvm.s390.vstrczbs
    s390_vstrczf,                              // llvm.s390.vstrczf
    s390_vstrczfs,                             // llvm.s390.vstrczfs
    s390_vstrczh,                              // llvm.s390.vstrczh
    s390_vstrczhs,                             // llvm.s390.vstrczhs
    s390_vstrl,                                // llvm.s390.vstrl
    s390_vstrsb,                               // llvm.s390.vstrsb
    s390_vstrsf,                               // llvm.s390.vstrsf
    s390_vstrsh,                               // llvm.s390.vstrsh
    s390_vstrszb,                              // llvm.s390.vstrszb
    s390_vstrszf,                              // llvm.s390.vstrszf
    s390_vstrszh,                              // llvm.s390.vstrszh
    s390_vsumb,                                // llvm.s390.vsumb
    s390_vsumgf,                               // llvm.s390.vsumgf
    s390_vsumgh,                               // llvm.s390.vsumgh
    s390_vsumh,                                // llvm.s390.vsumh
    s390_vsumqf,                               // llvm.s390.vsumqf
    s390_vsumqg,                               // llvm.s390.vsumqg
    s390_vtm,                                  // llvm.s390.vtm
    s390_vuphb,                                // llvm.s390.vuphb
    s390_vuphf,                                // llvm.s390.vuphf
    s390_vuphg,                                // llvm.s390.vuphg
    s390_vuphh,                                // llvm.s390.vuphh
    s390_vuplb,                                // llvm.s390.vuplb
    s390_vuplf,                                // llvm.s390.vuplf
    s390_vuplg,                                // llvm.s390.vuplg
    s390_vuplhb,                               // llvm.s390.vuplhb
    s390_vuplhf,                               // llvm.s390.vuplhf
    s390_vuplhg,                               // llvm.s390.vuplhg
    s390_vuplhh,                               // llvm.s390.vuplhh
    s390_vuplhw,                               // llvm.s390.vuplhw
    s390_vupllb,                               // llvm.s390.vupllb
    s390_vupllf,                               // llvm.s390.vupllf
    s390_vupllg,                               // llvm.s390.vupllg
    s390_vupllh,                               // llvm.s390.vupllh
}; // enum
} // namespace llvm::Intrinsic
#endif

