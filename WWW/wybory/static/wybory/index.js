
var LoginBar = React.createClass({

    getInitialState: function () {
        return {loggedIn: false};
    },

    componentDidMount: function () {
        $.ajax({
        url: '/user',
        cache: false,
        success: function(data) {
            if (data.auth)
                this.setState({loggedIn: true, user: data.user});
            else
                this.setState({loggedIn: false});
        }.bind(this),
    });
    },

    render: function () {
        if (this.state.loggedIn)
            return (<a href="/api-auth/logout/?next=/">Witaj {this.state.user}, wyloguj</a>)
        else
            return (<a href="/api-auth/login/?next=/">Zaloguj się</a>)
    }
});

ReactDOM.render(
    <LoginBar />,
    $('#react_login')[0]
);


var TitleHeader = React.createClass({
    render: function () {
        return (
            <div className="col-md-12 padding-zero">
                <div className="row padding-zero">
                    <div className="col-md-2 title_class">
                        <h4>
                            Wyniki głosowania
                        </h4>
                    </div>
                </div>
            </div>
        )
    }
});

var Map = React.createClass({

   render: function () {
       var viewBox = [0, 0, 130000, 120000].join(' ');
       var pathElems = paths.map(function(path, i){
           var classname = "map-path path" + i;
           var woj = this.props.wojewodztwa.find(function(e){
               return e.idw == i;
           });
           if (woj == null)
               woj = this.props.wojewodztwa[0];
           var top = (woj.l_glosow_1 >= woj.l_glosow_2) ? [1, woj.l_glosow_1] : [2, woj.l_glosow_2];
           var proc = top[1] / woj.l_waznych_glosow * 100;
           var colors = this.props.map_colors.find(function (e) {
               return ((e[3] <= proc) && (proc <= e[4]));
           });
           if (colors == null)
               colors = this.props.map_colors[0];
           var fill = (top[0] == 1) ? colors[1] : colors[2];
           return (
               <path
                   key={i}
                   className={classname}
                   d={path}
                   fill={fill}
               />
           )
       }.bind(this));
       return (
           <div className="col-md-7 map-div">
               <svg className="map-svg" viewBox={viewBox}>
                   {pathElems}
                </svg>
           </div>
       )
   }
});

var SummaryContent = React.createClass({
    render: function () {
        return (
            <div className="col-md-5 padding-zero">
                <div className="row">
                    <div className="col-md-12 map-polska">
                        <a href="#">
                            <h3 className="summary-content-header">Polska</h3>
                        </a>
                    </div>
                    <div className="col-md-12 padding-1">
                        <div className="table-responsive" >
                            <table className="table" >
                                <tbody>
                                    <tr className="summary-content-row-background">
                                        <th colSpan="2">Statystyka, stan na 30.06.2005</th>
                                    </tr>
                                    <tr>
                                        <td>Liczba mieszkańców:</td>
                                        <td><strong>{this.props.summary.l_mieszkancow}</strong></td>
                                    </tr>
                                    <tr>
                                        <td>Powierzchnia:</td>
                                        <td><strong>{this.props.summary.powierzchnia} km<sup>2</sup></strong></td>
                                    </tr>
                                    <tr>
                                        <td>Zaludnienie:</td>
                                        <td><strong>{this.props.summary.zaludnienie} os/km<sup>2</sup></strong></td>
                                    </tr>
                                    <tr className="summary-content-row-background">
                                        <th colSpan="2">Zbiorcze wyniki głosowania</th>
                                    </tr>
                                    <tr>
                                        <td>Liczba uprawnionych:</td>
                                        <td><strong>{this.props.summary.l_uprawnionych}</strong></td>
                                    </tr>
                                    <tr>
                                        <td>Liczba wydanych kart:</td>
                                        <td><strong>{this.props.summary.l_wydanych_kart}</strong></td>
                                    </tr>
                                    <tr>
                                        <td>Liczba głosów oddanych:</td>
                                        <td><strong>{this.props.summary.l_oddanych_glosow}</strong></td>
                                    </tr>
                                    <tr>
                                        <td>Liczba głosów ważnych:</td>
                                        <td><strong>{this.props.summary.l_waznych_glosow}</strong></td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
});

var MapLegend = React.createClass({
   render: function () {
       var elems = this.props.map_colors.map(function(e, i){
            var viewBox = [0, 0, 1, 1].join(' ');
            var fill1 = e[1];
           var fill2 = e[2];
            return (
                <div key={i} className="col-md-1 col-lg-1 padding-zero">
                        <div className="row">
                            <div className="col-lg-12 col-md-12 col-sm-4 col-xs-4 map-desc-a">
                                <svg viewBox={viewBox} preserveAspectRatio="xMinYMin slice">
                                    <rect x="0" y="0" width="1" height="1" stroke="none" fill={fill1} />
                                </svg>
                            </div>
                            <div className="col-lg-12 col-md-12 col-sm-4 col-xs-4 map-desc-b">
                                <svg viewBox={viewBox} preserveAspectRatio="xMinYMin slice">
                                    <rect x="0" y="0" width="1" height="1" stroke="none" fill={fill2} />
                                </svg>
                            </div>
                            <div className="col-lg-12 col-md-12 col-sm-4 col-xs-4 text-center padding-zero font-x-small">
                                {$("<div/>").html(e[0]).text()}
                            </div>
                        </div>
                </div>
            );
       }.bind(this));
       return (
            <div className="row">
                {elems}
            </div>
       )
   }
});

var Summary = React.createClass({
    render: function () {
        return (
            <div className="col-md-7 col-sm-7 col-xs-12">
            <section className="panel panel-default summary-section">
                <span className="summary-section-span">
                    Porównanie wyników głosowania na kandydatów
                </span>
                <ol className="breadcrumb summary-bread">
                    <li className="active">
                        <a href="#">Polska</a>
                    </li>
                    <li>
                        <a href="#">...</a>
                    </li>
                </ol>
                <div class="row">
                    <div className="row">
                        <Map
                            summary={this.props.summary}
                            map_colors={this.props.map_colors}
                            wojewodztwa={this.props.wojewodztwa}
                        />
                        <SummaryContent
                            summary={this.props.summary}
                        />
                    </div>
                    <MapLegend
                        map_colors={this.props.map_colors}
                    />
                </div>
            </section>
            </div>
        )
    }
});

var LinksSection = React.createClass({
    render: function () {
        return (
            <div className="col-md-5 col-sm-5 col-xs-12 no-print">
                <section className="panel panel-default links-section">
                    <ul className="list-unstyled">
                        <li><h5>Zobacz także:</h5>
                            <ul>
                                <li><a href="#">Frekwencja</a></li>
                                <li><a href="#">Wyniki głosowania zagranicą</a></li>
                                <li><a href="#">Porównanie wyników głosowania na kandydatów w zależności od siedziby komisji obwodowej</a></li>
                            </ul>
                        </li>
                        <li><h5>Szczegółowe wyniki głosowania na kandydatów:</h5>
                            <ul>
                                <li><a href="#">{this.props.summary.kandydat1.nazwisko.toUpperCase()} {this.props.summary.kandydat1.imie}</a></li>
                                <li><a href="#">{this.props.summary.kandydat2.nazwisko.toUpperCase()} {this.props.summary.kandydat2.imie}</a></li>
                            </ul>
                        </li>
                    </ul>
                </section>
            </div>
        );
    }
});

var Results = React.createClass({
    render: function () {
        var k1_proc = this.props.summary.l_glosow_1 / this.props.summary.l_waznych_glosow * 100;
        k1_proc = k1_proc.toFixed(2);
        var k2_proc = 100 - k1_proc;
        var width_style1 = {
            width: "" + (k1_proc) + "%"
        };
        var width_style2 = {
            width: "" + (k2_proc) + "%"
        };
        return (
            <div className="col-md-5 col-sm-5 col-xs-12">
                <section className="panel panel-default width-100">
                    <div className="row margin-0 text-center">
                        <h3 className="col-md-12 font-bold">{this.props.summary.kandydat1.nazwisko.toUpperCase()} {this.props.summary.kandydat1.imie}</h3>
                        <div className="col-md-6 col-sm-12 progress padding-0 margin-bottom-0">
                            <div className="progress-bar results-color-1" role="progressbar" style={width_style1}>
                                <span className="sr-only">{k1_proc}% Complete</span>
                            </div>
                        </div>
                        <div className="col-md-3 col-sm-6 col-xs-6"><h4 className="results-h4-style">{k1_proc}%</h4></div>
                        <div className="col-md-3 col-sm-6 col-xs-6"><h4 className="results-h4-style">{this.props.summary.l_glosow_1}</h4></div>
                    </div>

                    <div className="row margin-0 text-center">
                        <h3 className="col-md-12 font-bold">{this.props.summary.kandydat2.nazwisko.toUpperCase()} {this.props.summary.kandydat2.imie}</h3>
                        <div className="col-md-6 col-sm-12 progress padding-0">
                            <div className="progress-bar results-color-2" role="progressbar" style={width_style2}>
                                <span className="sr-only">{k2_proc}% Complete</span>
                            </div>
                        </div>
                        <div className="col-md-3 col-sm-6 col-xs-6"><h4 className="results-h4-style">{k2_proc}%</h4></div>
                        <div className="col-md-3 col-sm-6 col-xs-6"><h4 className="results-h4-style">{this.props.summary.l_glosow_2}</h4></div>
                    </div>
                </section>
            </div>
        )
    }
});

var Table1 = React.createClass({

    render: function () {
        var k1_proc = this.props.summary.l_glosow_1 / this.props.summary.l_waznych_glosow * 100;
        k1_proc = k1_proc.toFixed(2);
        var k2_proc = 100 - k1_proc;
        var width_style1 = {
            width: "" + (k1_proc) + "%"
        };
        var rows = this.props.wojewodztwa.map(function (w, i) {
            var w_proc1 = (w.l_glosow_1 / w.l_waznych_glosow * 100).toFixed(2);
            var w_proc2 = (100 - w_proc1).toFixed(2);
            var w_width_style1 = {
                width: "" + (w_proc1) + "%"
            };
            return (
                <tr key={i}>
                    <td colSpan="3">{w.idw}</td>
                    <td className="text-left">
                        <a className="cursor-pointer text-left-important" onClick={this.props.runModal}>
                            {w.nazwa}
                        </a>
                    </td>
                    <td>{w.l_waznych_glosow}</td>
                    <td>{w.l_glosow_1 }</td>
                    <td>{w_proc1}</td>
                    <td>
                        <div className="progress t1-progress">
                          <div className="progress-bar t1-progress-bar" style={w_width_style1}>
                            <span className="print-only">-</span>
                          </div>
                        </div>
                    </td>
                    <td>{w_proc2}</td>
                    <td colSpan="3">{w.l_glosow_2}</td>
                </tr>
            );
        }.bind(this));
        return (
            <div className="col-md-12 col-sm-12 col-xs-12">
                <section className="panel panel-default t1-section">
                    <span className="t1-section-span">Porównanie wyników głosowania na kandydatów</span>
                    <div className="table-responsive">
                    <table className="table table-hover tab1" id="firstTable">
                        <tbody>
                            <tr>
                                <th colSpan="3" rowSpan="3">Nr</th>
                                <th rowSpan="3">Nazwa</th>
                                <th rowSpan="3">Liczba głosów<br/>ważnych</th>
                                <th colSpan="2">{this.props.summary.kandydat1.nazwisko.toUpperCase()} {this.props.summary.kandydat1.imie}</th>
                                <th rowSpan="3">Liczba głosów:<br/>na kandydata / ważnych<br/>[%]</th>
                                <th colSpan="4">{this.props.summary.kandydat2.nazwisko.toUpperCase()} {this.props.summary.kandydat2.imie}</th>
                            </tr>
                            <tr>
                                <th colSpan="2">głosów na kandydata</th>
                                <th colSpan="4">głosów na kandydata</th>
                            </tr>
                            <tr>
                                <th>liczba</th>
                                <th>%</th>
                                <th>%</th>
                                <th colSpan="3">liczba</th>
                            </tr>

                            {rows}

                            <tr className="sum-up t1-sumup">
                                <th className="text-center" colSpan="3">&Sigma;</th>
                                <th className="text-center">Polska</th>
                                <th>{this.props.summary.l_waznych_glosow}</th>
                                <th>{this.props.summary.l_glosow_1}</th>
                                <th>{k1_proc}</th>
                                <th>
                                    <div className="progress t1-progress">
                                      <div className="progress-bar t1-progress-bar" style={width_style1}>
                                        <span className="print-only">-</span>
                                      </div>
                                    </div>
                                </th>
                                <th>{k2_proc}</th>
                                <th colSpan="3">{this.props.summary.l_glosow_2}</th>
                            </tr>
                        </tbody>
                    </table>
                    </div>
                </section>
            </div>
        );
    }
});

var Table2 = React.createClass({

    render: function () {
        var k1_proc = this.props.summary.l_glosow_1 / this.props.summary.l_waznych_glosow * 100;
        k1_proc = k1_proc.toFixed(2);
        var k2_proc = 100 - k1_proc;
        var width_style1 = {
            width: "" + (k1_proc) + "%"
        };
        var rows = this.props.rodzaje.map(function (w, i) {
            var w_proc1 = (w.l_glosow_1 / w.l_waznych_glosow * 100).toFixed(2);
            var w_proc2 = (100 - w_proc1).toFixed(2);
            var w_width_style1 = {
                width: "" + (w_proc1) + "%"
            };
            {/*onclick="modal_start({{ w.idw }}, 'x', -2, -2)"*/}
            return (
                <tr key={i}>
                    <td colSpan="3">{i+1}</td>
                    <td className="text-left">
                        <a className="cursor-pointer text-left-important">
                            {w.rodzaj[1]}
                        </a>
                    </td>
                    <td>{w.l_waznych_glosow}</td>
                    <td>{w.l_glosow_1 }</td>
                    <td>{w_proc1}</td>
                    <td>
                        <div className="progress t1-progress">
                          <div className="progress-bar t1-progress-bar" style={w_width_style1}>
                            <span className="print-only">-</span>
                          </div>
                        </div>
                    </td>
                    <td>{w_proc2}</td>
                    <td colSpan="3">{w.l_glosow_2}</td>
                </tr>
            );
        }.bind(this));
        var rows2 = this.props.rozmiary.map(function (w, i) {
            var w_proc1 = (w.l_glosow_1 / w.l_waznych_glosow * 100).toFixed(2);
            var w_proc2 = (100 - w_proc1).toFixed(2);
            var w_width_style1 = {
                width: "" + (w_proc1) + "%"
            };
            {/*onclick="modal_start({{ w.idw }}, 'x', -2, -2)"*/}
            return (
                <tr key={i}>
                    <td colSpan="3">{i+1}</td>
                    <td className="text-left">
                        <a className="cursor-pointer text-left-important">
                            {w.rozmiar[1]}
                        </a>
                    </td>
                    <td>{w.l_waznych_glosow}</td>
                    <td>{w.l_glosow_1 }</td>
                    <td>{w_proc1}</td>
                    <td>
                        <div className="progress t1-progress">
                          <div className="progress-bar t1-progress-bar" style={w_width_style1}>
                            <span className="print-only">-</span>
                          </div>
                        </div>
                    </td>
                    <td>{w_proc2}</td>
                    <td colSpan="3">{w.l_glosow_2}</td>
                </tr>
            );
        }.bind(this));
        return (
            <div className="col-md-12 col-sm-12 col-xs-12">
                <section className="panel panel-default t1-section">
                    <span className="t1-section-span">Porównanie wyników głosowania na kandydatów w zależności od siedziby komisji obwodowej</span>
                    <div className="table-responsive">
                    <table className="table table-hover tab1" id="firstTable">
                        <tbody>
                            <tr>
                                <th colSpan="3" rowSpan="3">Nr</th>
                                <th rowSpan="3">Rodzaj</th>
                                <th rowSpan="3">Liczba głosów<br/>ważnych</th>
                                <th colSpan="2">{this.props.summary.kandydat1.nazwisko.toUpperCase()} {this.props.summary.kandydat1.imie}</th>
                                <th rowSpan="3">Liczba głosów:<br/>na kandydata / ważnych<br/>[%]</th>
                                <th colSpan="4">{this.props.summary.kandydat2.nazwisko.toUpperCase()} {this.props.summary.kandydat2.imie}</th>
                            </tr>
                            <tr>
                                <th colSpan="2">głosów na kandydata</th>
                                <th colSpan="4">głosów na kandydata</th>
                            </tr>
                            <tr>
                                <th>liczba</th>
                                <th>%</th>
                                <th>%</th>
                                <th colSpan="3">liczba</th>
                            </tr>

                            {rows}

                            <tr className="no-hover">
                                <td colSpan="10" className="breaker">
                                    <span className="some-margins">
                                        Porównanie wyników głosowania na kandydatów w zależności od liczby mieszkańców gminy
                                    </span>
                                </td>
                            </tr>

                            {rows2}

                        </tbody>
                    </table>
                    </div>
                </section>
            </div>
        );
    }
});

var ReactRoot = React.createClass({

    getInitialState: function () {
        var ls = localStorage.getItem('state');
        if (ls)
            return JSON.parse(ls);
        else
            return {
            flip: false,
            summary: {
                "powierzchnia": "0 km<sup>2</sup>",
                "zaludnienie": "0 os/km<sup>2</sup>",
                "kandydat1": {
                    "nazwisko": "-", "imie": "-"
                },
                "kandydat2": {
                    "nazwisko": "-", "imie": "-"
                },
                "l_mieszkancow": 0,
                "l_uprawnionych": 0,
                "l_wydanych_kart": 0,
                "l_oddanych_glosow": 0,
                "l_waznych_glosow": 0,
                "l_glosow_1": 0,
                "l_glosow_2": 0,
                "flip": false
            },
            map_colors: [
                ["brak danych","#FFFFFF","#FFFFFF",null,null],
                ["50.00%","#BEFFA7","#BEFFA7",50.0,50.0],
                [">50.00%","#cce3ff","#ffdb70",50.001,52.269],
                ["&ge;52.27%","#b4d5fd","#ffcd70",52.27,54.539],
                ["&ge;54.54%","#99c7ff","#ffc15e",54.54,56.809],
                ["&ge;56.81%","#7db7fe","#ffb554",56.81,59.079],
                ["&ge;59.08%","#5aa4fe","#ffa951",59.08,61.349],
                ["&ge;61.35%","#3590ff","#ff9d39",61.35,63.619],
                ["&ge;63.62%","#0273fd","#fe9020",63.62,65.889],
                ["&ge;65.89%","#0260d4","#ff8609",65.89,68.159],
                ["&ge;68.16%","#014aa3","#e77900",68.16,70.429],
                ["&ge;70.43%","#003575","#ce6800",70.43,100.0]
            ],
            wojewodztwa: [
                {
                    "idw": 0,
                    "nazwa": "wojewodztwo",
                    "l_mieszkancow": 0,
                    "l_uprawnionych": 0,
                    "l_wydanych_kart": 0,
                    "l_oddanych_glosow": 0,
                    "l_waznych_glosow": 0,
                    "l_glosow_1": 0,
                    "l_glosow_2": 0
                }
            ],
            rodzaje: [
                {
                    "rodzaj": [
                        "m",
                        "Miasto"
                    ],
                    "l_mieszkancow": 0,
                    "l_uprawnionych": 0,
                    "l_wydanych_kart": 0,
                    "l_oddanych_glosow": 0,
                    "l_waznych_glosow": 0,
                    "l_glosow_1": 0,
                    "l_glosow_2": 0
                }
            ],
            rozmiary: [
                {
                    "rozmiar": [
                        "1",
                        "Statki i zagranica",
                        -1,
                        -1
                    ],
                    "l_mieszkancow": 0,
                    "l_uprawnionych": 0,
                    "l_wydanych_kart": 0,
                    "l_oddanych_glosow": 0,
                    "l_waznych_glosow": 0,
                    "l_glosow_1": 0,
                    "l_glosow_2": 0
                }
            ]
        };
    },

    runModal: function () {
        $('#myModal').modal();
    },

    componentDidMount: function() {
        $.ajax({
          url: '/summary/',
          dataType: 'json',
          cache: false,
          success: function(data) {
              if (data.flip) {
                  this.setState({flip: true});
                  data.l_glosow_2 = [data.l_glosow_1, data.l_glosow_1 = data.l_glosow_2][0];
                  data.kandydat2 = [data.kandydat1, data.kandydat1 = data.kandydat2][0];
              }
              this.setState({summary: data});
          }.bind(this),
          error: function(xhr, status, err) {
            console.error(this.props.url, status, err.toString());
          }.bind(this)
        });
        $.ajax({
          url: '/map_colors/',
          dataType: 'json',
          cache: false,
          success: function(data) {
            this.setState({map_colors: data});
          }.bind(this),
          error: function(xhr, status, err) {
            console.error(this.props.url, status, err.toString());
          }.bind(this)
        });
        $.ajax({
          url: '/wojewodztwa/',
          dataType: 'json',
          cache: false,
          success: function(data) {
              if (this.state.flip) {
                  data = data.map(function(x){
                      x.l_glosow_1 = [x.l_glosow_2, x.l_glosow_2 = x.l_glosow_1][0];
                      return x;
                  })
              }
            this.setState({wojewodztwa: data});
          }.bind(this),
          error: function(xhr, status, err) {
            console.error(this.props.url, status, err.toString());
          }.bind(this)
        });
        $.ajax({
          url: '/rodzaje/',
          dataType: 'json',
          cache: false,
          success: function(data) {
              if (this.state.flip) {
                  data = data.map(function(x){
                      x.l_glosow_1 = [x.l_glosow_2, x.l_glosow_2 = x.l_glosow_1][0];
                      return x;
                  })
              }
            this.setState({rodzaje: data});
          }.bind(this),
          error: function(xhr, status, err) {
            console.error(this.props.url, status, err.toString());
          }.bind(this)
        });
        $.ajax({
          url: '/rozmiary/',
          dataType: 'json',
          cache: false,
          success: function(data) {
              if (this.state.flip) {
                  data = data.map(function(x){
                      x.l_glosow_1 = [x.l_glosow_2, x.l_glosow_2 = x.l_glosow_1][0];
                      return x;
                  })
              }
            this.setState({rozmiary: data});
          }.bind(this),
          error: function(xhr, status, err) {
            console.error(this.props.url, status, err.toString());
          }.bind(this)
        });

    },

    componentDidUpdate: function () {
        localStorage.setItem('state', JSON.stringify(this.state));
    },

    render: function () {
       return (
           <div className="row">
               <TitleHeader force={this.forceUpdate} />

               <Summary
                   summary={this.state.summary}
                   map_colors={this.state.map_colors}
                   wojewodztwa={this.state.wojewodztwa}
               />

               <LinksSection summary={this.state.summary} />
               
               <Results summary={this.state.summary} />

                <Table1
                    summary={this.state.summary}
                    wojewodztwa={this.state.wojewodztwa}
                    runModal={this.runModal}
                />

               <Table2
                    summary={this.state.summary}
                    rodzaje={this.state.rodzaje}
                    rozmiary={this.state.rozmiary}
                />
           </div>
       )
   }
});

ReactDOM.render(
    <ReactRoot />,
    $('#react_root')[0]
);