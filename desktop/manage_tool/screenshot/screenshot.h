/*!
    屏幕截图的相关应用接口
*/

#ifndef _INCLUDE_SCREENSHOT_H
#define _INCLUDE_SCREENSHOT_H

#include <QMainWindow>
#include <QPixmap>
#include <QPoint>
#include <QtGui>
#include <QPainter>
#include <QBrush>
#include <QRect>
#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>

class CScreenShot : public QMainWindow
{
  Q_OBJECT

  public:
  CScreenShot(); //构造函数

  enum shotState{
      initShot,
      beginShot,
      finishShot,
      endShot,
      beginMoveShot,
      finishMoveShot,
      beginControl,
      finishControl
  }; //进行截屏的状态

  //移动选区中的8个控制点，按照顺时针方向从左上控制点到左中控制点分配编号为1～8
  enum controlPointEnum{
      moveControl0,
      moveControl1,
      moveControl2,
      moveControl3,
      moveControl4,
      moveControl5,
      moveControl6,
      moveControl7,
      moveControl8
  };

  QPixmap getFullScreenPixmap(); //获取全屏的Pixmap

public slots:
  void loadBackgroundPixmap(const QPixmap &bgPixmap);//加载背景Pixmap槽函数
  void loadBackgroundPixmap(const QPixmap &bgPixmap, int x, int y, int width, int height); //加载背景pixmap槽函数，设置x,y,width,height
  void cancelSelectedRect(); //取消选择区域
  void savePixmap();             //保选取行为的方法
  void SetMainWinStatus(bool Status);

signals:
  void send_release(void);

private:
  //选区框的8个点选取
  QRect tlRect; //左上点
  QRect trRect; //右上点
  QRect blRect; //左下点
  QRect brRect; //右下点
  QRect tcRect; //上中点
  QRect bcRect; //下中点
  QRect lcRect;//左中点
  QRect rcRect; //右中点
  int isMainWinHide;

  QPainter painter;
  QPoint beginPoint,endPoint,moveBeginPoint,moveEndPoint;
  QRect selectedRect; //选择区域
  QPixmap loadPixmap,shotPixmap;
  shotState currentShotState; //当前的截屏状态
  controlPointEnum controlValue; //记录移动控制点的值
  QAction *savePixmapAction; //保存图片行为
  QAction *cancelAction; //取消选取行为
  QAction *quitAction; //退出选取行为
  QMenu *contextMenu; //选中区域右键菜单
  int screenwidth; //整个屏幕的宽度
  int screenheight; //整个屏幕的高度
  int screenx; //选区的X
  int screeny; //选区的Y
  int tipWidth,tipHeight,infoWidth,infoHeight; //加载初始框的宽度，高度；显示坐标信息的宽度，高度

  QRect getSelectedRect(); //获取选取
  QRect getRect(const QPoint &beginPoint, const QPoint &endPoint); //根据两个点获取选取坐标
  void initCScreenShot(); //初始化抓全屏的相关参数
  bool isInSelectedRect(const QPoint &point); //判断该点是否在选中区域
  void initSelectedMenu();//初始化右键菜单
  void drawTipsText(); //在屏幕上打印提示信息
  void drawSelectedPixmap(void); //在屏幕上画选取的屏幕
  void updateBeginEndPointValue(const QRect &rect);  //当移动选取后，对beginPoint,endPoint坐标进行重新修改
  void checkMoveEndPoint(); //对移动的选区进行判断
  void draw8ControlPoint(const QRect &rect);
  void updateMouseShape(const QPoint &point); //更新鼠标的当前状态
  void updateMoveControlMouseShape(controlPointEnum controlValue);
  controlPointEnum getMoveControlState(const QPoint &point); //获取移动控制点状态
  QRect getMoveAllSelectedRect(void); //获取移动整个选中的选区
  QRect getMoveControlSelectedRect(void);//获取移动控制点的选区
  int getMinValue(int num1, int num2);//获取两个数中的最小值
  void drawXYWHInfo(void); //打印选取的x,y,h,w值信息
  void Release(void);

  //重写基类方法
  void keyPressEvent(QKeyEvent *event);
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
};

void ScreenShotInit(void);
CScreenShot *GetScrenShotInfo(void);

#endif
