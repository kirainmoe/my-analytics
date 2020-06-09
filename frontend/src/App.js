import React from 'react';
import echarts from 'echarts';
import { Skeleton, message, Table } from "antd";
import registerTheme from "./echartTheme";

import './App.css';
import 'antd/dist/antd.css';

const getQueryString = (key) => {
  const uri = decodeURI(window.location.search.substring(1)).split('&'),
    mapper = {};
  uri.forEach(item => {
    const tmp = item.split('=');
    mapper[tmp[0]] = tmp[1];
  });
  return mapper[key];
};

class App extends React.Component {
  state = {
    domain: getQueryString("domain"),
    isLoading: true,
    totalVisit: 0,
    dependentVisit: 0,
    regionCount: 0,
    data: undefined
  }

  componentDidMount() {
    fetch(`/cgi-bin/analytics.cgi?domain=${this.state.domain}`)
      .then(res => res.json())
      .then(data => {
        let dependentVisit = 0;
        for (const r in data.region) {
          if (data.region.hasOwnProperty(r))
            dependentVisit += Number(data.region[r]);
        }
        this.setState({
          isLoading: false,
          totalVisit: data.rawData.length,
          dependentVisit,
          regionCount: Object.keys(data.region).length,
          data
        });
        this.renderData(data);
      })
      .catch(err => {
        message.error("Failed to get statistic data dut to network error.");
        console.error(err);
      });
  }

  renderData(data) {
    registerTheme(echarts);

    const browserChart = echarts.init(document.getElementById("browsers-chart"), "westeros");
    const browserChartData = [];
    for (const browser in data.browser) {
      if (data.browser.hasOwnProperty(browser)) {
        browserChartData.push({
          name: browser,
          value: data.browser[browser]
        });
      }
    }
    const browserChartOption = {
      title: [{
        text: "访问者使用的浏览器"
      }],
      legend: {
        orient: 'vertical',
        bottom: 0,
        left: 0,
        data: Object.keys(data.browser)
      },
      tooltip: {
        trigger: 'item',
        formatter: '{b} : {c} ({d}%)'
      },
      series: [{
        type: 'pie',
        data: browserChartData,
        animation: true,
        radius: ['50%', '70%'],
        avoidLabelOverlap: false,
        right: 0,
        label: {
          show: false,
          position: 'center'
        },
        emphasis: {
          label: {
            show: true,
            fontSize: '30',
            fontWeight: 'bold'
          }
        },
        labelLine: {
          show: false
        }
      }]
    };
    browserChart.setOption(browserChartOption);

    const systemChart = echarts.init(document.getElementById("system-chart"), "westeros");
    const systemChartData = [];
    for (const sys in data.system) {
      if (data.system.hasOwnProperty(sys)) {
        systemChartData.push({
          name: sys,
          value: data.system[sys]
        });
      }
    }
    const systemChartOption = {
      title: [{
        text: "访问者使用的操作系统"
      }],
      legend: {
        orient: 'vertical',
        bottom: 0,
        left: 0,
        data: Object.keys(data.browser)
      },
      tooltip: {
        trigger: 'item',
        formatter: '{b} : {c} ({d}%)'
      },
      series: [{
        type: 'pie',
        data: browserChartData,
        animation: true,
        radius: ['50%', '70%'],
        avoidLabelOverlap: false,
        right: 0,
        label: {
          show: false,
          position: 'center'
        },
        emphasis: {
          label: {
            show: true,
            fontSize: '30',
            fontWeight: 'bold'
          }
        },
        labelLine: {
          show: false
        }
      }]
    };
    systemChart.setOption(systemChartOption);

    const regionChart = echarts.init(document.getElementById("region-chart"), "westeros");
    const regionChartTags = [], regionChartData = [];
    for (const r in data.region) {
      if (data.region.hasOwnProperty(r)) {
        regionChartTags.push(r);
        regionChartData.push(data.region[r]);
      }
    }
    const regionChartOption = {
      title: {
        text: '访问者来自的地区'
      },
      tooltip: {
        trigger: 'axis',
        axisPointer: {
          type: 'shadow'
        }
      },
      grid: {
        left: '3%',
        right: '4%',
        bottom: '3%',
        containLabel: true
      },
      xAxis: {
        type: 'value',
        boundaryGap: [0, 0.01]
      },
      yAxis: {
        type: 'category',
        data: regionChartTags
      },
      series: [
        {
          name: 'from',
          type: 'bar',
          data: regionChartData
        }
      ]
    };
    regionChart.setOption(regionChartOption);

    const languageChart = echarts.init(document.getElementById("language-chart"), "westeros");
    const languageChartData = [];
    for (const l in data.language) {
      if (data.language.hasOwnProperty(l)) {
        languageChartData.push({
          name: l,
          value: data.language[l]
        });
      }
    }
    const languageChartOption = {
      title: [{
        text: "访问者使用的语言"
      }],
      legend: {
        orient: 'vertical',
        bottom: 0,
        left: 0,
        data: Object.keys(data.language)
      },
      tooltip: {
        trigger: 'item',
        formatter: '{b} : {c} ({d}%)'
      },
      series: [{
        type: 'pie',
        data: languageChartData,
        animation: true,
        avoidLabelOverlap: false,
        right: 0,
        label: {
          show: false,
          position: 'center'
        },
        emphasis: {
          label: {
            show: true,
            fontSize: '30',
            fontWeight: 'bold'
          }
        },
        labelLine: {
          show: false
        }
      }]
    };
    languageChart.setOption(languageChartOption)
  }

  renderTable() {
    if (this.state.isLoading)
      return;
    const dataSource = [];
    const columns = [
      {
        title: "页面地址",
        dataIndex: 'url',
        key: 'url',
        ellipsis: true
      },
      {
        title: "访问次数",
        dataIndex: 'visit',
        key: 'visit'
      }
    ];
    let index = 0;
    for (const i in this.state.data.pages) {
      if (!this.state.data.pages.hasOwnProperty(i))
        continue;
      index++;
      dataSource.push({
        key: index,
        url: i,
        visit: this.state.data.pages[i]
      });
    }
    dataSource.sort((a, b) => {
      if (a.visit > b.visit)
        return -1;
      if (a.visit === b.visit)
        return 0;
      return 1;
    });
    return <Table style={{ width: '100%' }} dataSource={dataSource} columns={columns} />
  }

  render() {
    return (
      <div className={"my-analytics-container"}>
        <h1 className={"title"}>访客统计报告</h1>
        <p className={"subtitle"}>域名：{this.state.domain}</p>
        <div className={"data-container"}>
          <div className={"sketelon-container"} style={{
            display: this.state.isLoading ? 'block' : 'none'
          }}>
            <Skeleton avatar paragraph={{ rows: 4 }} active/>
            <Skeleton avatar paragraph={{ rows: 4 }} active/>
          </div>

          <div className={"charts"} style={{
            display: this.state.isLoading ? 'none' : 'block'
          }}>
            <div className={"number-statistic"}>
              <h2>访问统计</h2>
              <p>总访问次数：{this.state.totalVisit}</p>
              <p>独立访客数：{this.state.dependentVisit}</p>
              <p>地区数：{this.state.regionCount}</p>

              <h2>最常被访问的页面</h2>
              <div className={"table-container"}>
                {this.renderTable()}
              </div>
            </div>
            <div
              className={"chart-container"}
              id={"browsers-chart"}
              style={{height: 350}} />
            <div
              className={"chart-container"}
              id={"system-chart"}
              style={{height: 350}} />
            <div
              className={"chart-container"}
              id={"region-chart"}
              style={{height: 500}} />
            <div
              className={"chart-container"}
              id={"language-chart"}
              style={{height: 350}} />
          </div>
        </div>
        <div className={"footer"}>
          Powered by My-Analytics (XMU 2020 Linux Final Project) <br />
          &copy;2020 Yume Maruyama, all rights reserved.
        </div>
      </div>
    );
  }
}

export default App;
