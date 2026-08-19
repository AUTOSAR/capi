..
    Disclaimer

    This work (specification and/or software implementation) and the material
    contained in it, as released by AUTOSAR, is for the purpose of information
    only. AUTOSAR and the companies that have contributed to it shall not be
    liable for any use of the work.

    The material contained in this work is protected by copyright and other
    types of intellectual property rights. The commercial exploitation of the
    material contained in this work requires a license to such intellectual
    property rights.

    This work may be utilized or reproduced without any modification, in any
    form or by any means, for informational purposes only. For any other
    purpose, no part of the work may be utilized or reproduced, in any form
    or by any means, without permission in writing from the publisher.

    The work has been developed for automotive applications only. It has
    neither been developed, nor tested for non-automotive applications.

    The word AUTOSAR and the AUTOSAR logo are registered trademarks.
    --------------------------------------------------------------------------

===============================
Source Code Documentation Style
===============================

Preface
=======

This document describes the ruleset that the CAPI community has given itself.
If you find a rule that should be discussed or think something is missing, open an improvement or bug ticket against this document.
The document is maintained by SDE <sde@autosar.org>.

In this document the rules regarding the style of documentation within the source code is detailed.

Existing style may be kept, but newly created documentation shall follow the guideline as described below.
When existing doxygen comments are changed, the style should be adapted.
An update of existing styles to this style is always welcome.

Doxygen Comment Style
=====================

The CAPI project uses the following style for multiline doxygen comments::

    /**
    *
    *
    */


The alternative ``///`` is not used, because the block-comment syntax has better tool support for detecting that it is one larger comment.
E.g., it can be collapsed in many editors


Uptrace style
=============

The following style shall be used for new code:

``@uptrace{SWS_XX_YYYYY, RZZ-11}``
``RZZ-11`` refers to the release of the specification item that is referenced.

For references to multiple items, use multiple entries of that style so e.g.::

    @uptrace{SWS_XX_YYYY1, RZZ-11}
    @uptrace{SWS_XX_YYYY2, RZZ-11}

In case there are important constraints to the uptrace
(e.g., a certain part of the specification item is not implemented, or implemented differently),
use the style from the following example::

    @uptrace{SWS_XX_YYYYY, RZZ-11 but the implementation is not thread safe}

In the future this style may be adapted. A change to a different style can be done using a tool for automatic conversion.

Reasoning
---------

A unified style makes it

- easier for tooling to work with the references and to support the verification and consistency
- easier for human readers to understand the references

The proposed style contains the crucial information about the version of the specification item.
Without that, it is difficult to understand the reference.
It was decided to go for the simpler reference to a release instead of using the hash for now.
This is to make is possible to use the information even without dedicated tooling.
An upgrade to using hashes in the future can be done using a conversion tool.

Using separate annotations for each specification item makes it easier for tooling to pick up the references, since there is less variety in the format.
It also allows the addition of the version information for each item.

Allowing to document deviations supports the retention of important information.


.. admonition:: Background: Current formats
   :collapsible: closed

   Looking at the current implementation there is a variety of different styles used to reference specification items.
   The following is a list of examples showing different styles from the project.::

      @uptrace{SWS_CORE_08045, 8119c472c8ed399ec9541e63155648d65540cdaa}
      @uptrace{SWS_Crc_00058}
      @uptrace={SWS_UCM_00026, 243b4ac88c72a2c4fc6f59c3f46bf819f1031e19}
      @uptrace={SWS_VUCM_00298}
      @uptrace{SWS_CM_10218, E2E errors domain}
      @uptrace={SWS_UCM_01227, SWS_UCM_01236, SWS_UCM_01244, SWS_UCM_01245, SWS_UCM_01270, SWS_UCM_01271,
         /// SWS_UCM_01269}

      @ref [SWS_CM_99023]
      @ref [SWS_CM_00313] [SWS_CM_00314] [SWS_CM_00315]

      @trace_id_sws={SWS_CRYPT_10015}
      @trace_id_sws= {SWS_ANM_00066}
      @trace_id_sws={SWS_DM_00526}@tracestatus{draft}
      @trace_id_sws=SWS_EM_02314
      @trace_id_sws= {SWS_ANM_00009 SWS_ANM_00033 SWS_ANM_00034 SWS_ANM_00035
         /// SWS_ANM_00037 SWS_ANM_00038 SWS_ANM_00071 SWS_ANM_00091 SWS_ANM_00040}

      @traceid{SWS_DM_01530}@tracestatus{draft}
      @traceid {SWS_PER_00339}{SWS_PER_00331}
      @traceid {SWS_PER_00389 SWS_PER_00393}
      @traceid {SWS_PER_00365}
      @traceid{SWS_TS_00212}
      @traceid  {SWS_TS_01003, 20-11}

      @AUTOSAR_SWS {SWS_CRYPT_22100}

      @brief @AUTOSAR_SWS {SWS_PER_00443}
      @brief SWS_CRYPT_01207、SWS_CRYPT_22115
      @brief [SWS_CRYPT_01209]

      @SWS_E2E_00386

      @req<SWS_Crc_00022>

      req<sws_crc_00040></sws_crc_00040>
      [SWS_CRYPT_01209]
      [SWS_CM_90464]{DRAFT}
      [SWS_CM_10274] [SWS_CM_10276]
      (SWS_CM_00192)
      SWS_PER_00396
