import QtQuick 2.0
import StratifyLabs.UI 2.1
import QtCharts 2.2

SContainer {
  name: "Test";
  style: "block fill";

  SColumn {
    style: "block fill";
    SRow {
      SButton {
        span: 2;
        style: "btn-success text-demi-bold left";
        text: "Start Monitor";
        onClicked: {
          console.log("Start Monitor");
          if( logic.isAppRunning("ads1015-demo") === false ){
            logic.flushStream(1);
            channel0.removePoints(0, channel0.count);
            logic.runApp("ads1015-demo -o /dev/multistream --channel 1 -p " + periodLabel.text);
            console.log("Not running");
          } else {
            console.log("Running!");
          }
        }
      }

      SButton {
        span: 2;
        style: "btn-danger right text-demi-bold";
        text: "Stop Monitor";
        onClicked: {
          console.log("Stop Monitor");
          logic.killApp("ads1015-demo", "SIGQUIT");
        }
      }
    }


    SChart {
      id: temperatureChart;
      style: "text-h1 text-demi-bold legend-h3 legend-bottom fill";
      title: "Temperature Data";

      SJsonModel {
        id: seriesData;
        json: '{
    "data": [
      { "x": 0, "y": 0 },
      { "x": 1, "y": 1 },
      { "x": 2, "y": 4 },
      { "x": 3, "y": 9 },
      { "x": 4, "y": 16 },
      { "x": 5, "y": 25 },
      { "x": 6, "y": 36 }
    ]
  }';
      }

      SJsonModel {
        id: seriesData2;
        json: '{
    "data": [
      { "x": 0, "y": 0 },
      { "x": 1, "y": 3 },
      { "x": 2, "y": 5 },
      { "x": 3, "y": 10 },
      { "x": 4, "y": 17 },
      { "x": 5, "y": 26 },
      { "x": 6, "y": 37 }
    ]
  }';
      }

      SLineSeries {
        id: channel0;
        style: "success line-solid cap-flat";
        name: "Ch:0";
        //model: seriesData.model;

        axisX: ValueAxis {
          id: xAxis;
          min:0;
          max:200;
        }

        axisY: ValueAxis {
          id: yAxis;
          min: 20;
          max: 150;
        }

        onPointAdded: {
          xAxis.max = channel0.at(index).x + 10;
          xAxis.min = channel0.at(index).x - 40;
          if( xAxis.min < 0 ){
            xAxis.min = 0;
          }
        }

        Connections {
          target: logic;
          onStreamChannelJsonChanged: {
            if( channel == 1 ){ //channel 1 is data IN to the computer
              channel0.append(object.points[0].x, object.points[0].y);
              temperatureChart.setAxisX(xAxis, channel0);
              temperatureChart.setAxisY(yAxis, channel0);

            }
          }
        }
      }

    }

    SRow {
      SGroup {
        span: 4;
        style: "right";
        SButton {
          style: "btn-primary text-demi-bold left";
          text: "Export JSON";
          onClicked: {
            console.log("Start Monitor");
          }
        }

        SButton {
          style: "btn-primary right text-demi-bold";
          text: "Export CSV";
          onClicked: {

          }
        }

        SButton {
          style: "btn-primary right text-demi-bold";
          text: "Export Cloud";
          onClicked: {

          }
        }

        SButton {
          style: "btn-primary right text-demi-bold";
          text: "Install App";
          onClicked: {
            logic.installApp("/Users/tgil/git/StratifyApps/ads1015-demo");
          }
        }
      }
    }

    SColumn {
      SRow {
        SLabel {
          span: 2;
          style: "left";
          text: "Period";
        }

        SLabel {
          id: periodLabel;
          span: 2;
          style: "right";
          text: Math.round(periodSlider.value * 2000/50)*50;
        }

      }

      SSlider {
        id: periodSlider;
        style: "block primary";
      }
    }

  }
}
