/*
* OpenClonk, http://www.openclonk.org
*
* Copyright (c) 2001-2009, RedWolf Design GmbH, http://www.clonk.de/
* Copyright (c) 2013, The OpenClonk Team and contributors
*
* Distributed under the terms of the ISC license; see accompanying file
* "COPYING" for details.
*
* "Clonk" is a registered trademark of Matthes Bender, used with permission.
* See accompanying file "TRADEMARK" for details.
*
* To redistribute this file separately, substitute the full license texts
* for the above references.
*/

/* Player and editor viewports in console */

#ifndef INC_C4ConsoleQtViewport
#define INC_C4ConsoleQtViewport
#ifdef WITH_QT_EDITOR

#include <C4Include.h> // needed for automoc
#include <C4ConsoleGUI.h> // for glew.h
#include <C4ConsoleQt.h>

class C4ConsoleQtViewportView : public QWidget
{
	Q_OBJECT

	class C4ConsoleQtViewportDockWidget *dock;
	class C4Viewport *cvp;

private:
	bool IsPlayViewport() const;

protected:
	void focusInEvent(QFocusEvent * event) override;
	void focusOutEvent(QFocusEvent * event) override;
	void resizeEvent(QResizeEvent *resize_event) override;
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
	void mouseMoveEvent(QMouseEvent *eventMove) override;
	void mousePressEvent(QMouseEvent *eventPress) override;
	void mouseDoubleClickEvent(QMouseEvent *eventPress) override;
	void mouseReleaseEvent(QMouseEvent *releaseEvent) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent * event) override;
	void keyReleaseEvent(QKeyEvent * event) override;
	void enterEvent(QEvent *) override;
	void leaveEvent(QEvent *) override;

public:
	C4ConsoleQtViewportView(class C4ConsoleQtViewportDockWidget *dock);
};

class C4ConsoleQtViewportLabel : public QLabel
{
	Q_OBJECT

	class C4ConsoleQtViewportDockWidget *dock;
	bool active;
	QPalette pal_inactive, pal_active;

public:
	C4ConsoleQtViewportLabel(const QString &title, C4ConsoleQtViewportDockWidget *dock);
	void OnActiveChanged(bool active);

protected:
	void mousePressEvent(QMouseEvent *eventPress) override;
};

class C4ConsoleQtViewportDockWidget : public QDockWidget
{
	Q_OBJECT

	class C4ConsoleQtMainWindow *main_window;
	class C4ViewportWindow *cvp;
	C4ConsoleQtViewportView *view;
	C4ConsoleQtViewportLabel *title_label;

protected:
	void mousePressEvent(QMouseEvent *eventPress);
	void OnActiveChanged(bool active);

public:
	C4ConsoleQtViewportDockWidget(class C4ConsoleQtMainWindow *main_window, QMainWindow *parent, class C4ViewportWindow *window);
	virtual void closeEvent(QCloseEvent * event);
	class C4ViewportWindow *GetViewportWindow() const { return cvp; }
	void SetFocus() { view->setFocus(); } // forward focus to view

private slots :
	void TopLevelChanged(bool is_floating);

	friend C4ConsoleQtViewportView;
	friend C4ConsoleQtViewportLabel;
};


#endif // WITH_QT_EDITOR
#endif // INC_C4ConsoleQtViewport