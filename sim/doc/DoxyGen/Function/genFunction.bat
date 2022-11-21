:: /*********************************************************************
:: * This Example Content is intended to demonstrate usage of
:: * Eclipse technology. It is provided to you under the terms
:: * and conditions of the Eclipse Distribution License v1.0
:: * which is available at http://www.eclipse.org/org/documents/edl-v10.php
:: **********************************************************************/

rmdir /s /q html

set SourceDir=../../../src

set DocumentationDir=../

doxygen Function.doxy -d markdown

move /y html\openPASS_Function_Docu.chm ../openPASS_Function_Docu.chm
rmdir /s /q html
