/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#pragma once

#include <vclpluginapi.h>
#include <salusereventlist.hxx>

#include <osl/conditn.hxx>
#include <o3tl/make_unique.hxx>

#include <QtCore/QObject>

class QApplication;
class SalYieldMutex;
class SalFrame;

#if (defined MACOSX || defined _WIN32)
#include <salinst.hxx>
#include <comphelper/solarmutex.hxx>
#else
#include <unx/geninst.h>
#endif

// Qts moc doesn't like macros, so this is handled by an extra base class
// It also keeps all the #ifdef handling local
class VCLPLUG_QT5_PUBLIC Qt5MocInstance
#if (defined MACOSX || defined _WIN32)
    : public SalInstance
#else
    : public SalGenericInstance
#endif
{
public:
    Qt5MocInstance()
#if (defined MACOSX || defined _WIN32)
        : SalInstance(o3tl::make_unique<comphelper::SolarMutex>())
#else
        : SalGenericInstance(o3tl::make_unique<SalYieldMutex>())
#endif
    {
    }

#if !(defined MACOSX || defined _WIN32)
    virtual GenPspGraphics* CreatePrintGraphics() override;
    virtual void PostPrintersChanged() override;
#endif
};

class VCLPLUG_QT5_PUBLIC Qt5Instance : public QObject,
                                       public Qt5MocInstance,
                                       public SalUserEventList
{
    Q_OBJECT

    osl::Condition m_aWaitingYieldCond;
    int m_postUserEventId;
    const bool m_bUseCairo;

public:
    std::unique_ptr<QApplication> m_pQApplication;
    std::unique_ptr<char* []> m_pFakeArgvFreeable;
    std::unique_ptr<char* []> m_pFakeArgv;
    std::unique_ptr<int> m_pFakeArgc;

private Q_SLOTS:
    bool ImplYield(bool bWait, bool bHandleAllCurrentEvents);

Q_SIGNALS:
    bool ImplYieldSignal(bool bWait, bool bHandleAllCurrentEvents);

public:
    explicit Qt5Instance(bool bUseCairo = false);
    virtual ~Qt5Instance() override;

    virtual SalFrame* CreateFrame(SalFrame* pParent, SalFrameStyleFlags nStyle) override;
    virtual SalFrame* CreateChildFrame(SystemParentData* pParent,
                                       SalFrameStyleFlags nStyle) override;
    virtual void DestroyFrame(SalFrame* pFrame) override;

    virtual SalObject* CreateObject(SalFrame* pParent, SystemWindowData* pWindowData,
                                    bool bShow) override;
    virtual void DestroyObject(SalObject* pObject) override;

    virtual std::unique_ptr<SalVirtualDevice>
    CreateVirtualDevice(SalGraphics* pGraphics, long& nDX, long& nDY, DeviceFormat eFormat,
                        const SystemGraphicsData* pData = nullptr) override;

    virtual SalInfoPrinter* CreateInfoPrinter(SalPrinterQueueInfo* pQueueInfo,
                                              ImplJobSetup* pSetupData) override;
    virtual void DestroyInfoPrinter(SalInfoPrinter* pPrinter) override;
    virtual std::unique_ptr<SalPrinter> CreatePrinter(SalInfoPrinter* pInfoPrinter) override;
    virtual void GetPrinterQueueInfo(ImplPrnQueueList* pList) override;
    virtual void GetPrinterQueueState(SalPrinterQueueInfo* pInfo) override;
    virtual void DeletePrinterQueueInfo(SalPrinterQueueInfo* pInfo) override;
    virtual OUString GetDefaultPrinter() override;

    virtual std::unique_ptr<SalMenu> CreateMenu(bool, Menu*) override;
    virtual std::unique_ptr<SalMenuItem> CreateMenuItem(const SalItemParams&) override;

    virtual SalTimer* CreateSalTimer() override;
    virtual SalSystem* CreateSalSystem() override;
    virtual std::shared_ptr<SalBitmap> CreateSalBitmap() override;

    virtual bool DoYield(bool bWait, bool bHandleAllCurrentEvents) override;
    virtual bool AnyInput(VclInputFlags nType) override;

    virtual OpenGLContext* CreateOpenGLContext() override;

    virtual OUString GetConnectionIdentifier() override;

    virtual void AddToRecentDocumentList(const OUString& rFileUrl, const OUString& rMimeType,
                                         const OUString& rDocumentService) override;

    virtual bool IsMainThread() const override;

    virtual void TriggerUserEventProcessing() override;
    virtual void ProcessEvent(SalUserEvent aEvent) override;

    virtual css::uno::Reference<css::ui::dialogs::XFilePicker2>
    createFilePicker(const css::uno::Reference<css::uno::XComponentContext>&) override;

    virtual css::uno::Reference<css::ui::dialogs::XFolderPicker2>
    createFolderPicker(const css::uno::Reference<css::uno::XComponentContext>&) override;
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
