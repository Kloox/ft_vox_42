#include "Noiser.hpp"

const float CELL_3D_X[] =
    {
        float(0.3752498686),
        float(0.687188096),
        float(0.2248135212),
        float(0.6692006647),
        float(-0.4376476931),
        float(0.6139972552),
        float(0.9494563929),
        float(0.8065108882),
        float(-0.2218812853),
        float(0.8484661167),
        float(0.5551817596),
        float(0.2133903499),
        float(0.5195126593),
        float(-0.6440141975),
        float(-0.5192897331),
        float(-0.3697654077),
        float(-0.07927779647),
        float(0.4187757321),
        float(-0.750078731),
        float(0.6579554632),
        float(-0.6859803838),
        float(-0.6878407087),
        float(0.9490848347),
        float(0.5795829433),
        float(-0.5325976529),
        float(-0.1363699466),
        float(0.417665879),
        float(-0.9108236468),
        float(0.4438605427),
        float(0.819294887),
        float(-0.4033873915),
        float(-0.2817317705),
        float(0.3969665622),
        float(0.5323450134),
        float(-0.6833017297),
        float(0.3881436661),
        float(-0.7119144767),
        float(-0.2306979838),
        float(-0.9398873022),
        float(0.1701906676),
        float(-0.4261839496),
        float(-0.003712295499),
        float(-0.734675004),
        float(-0.3195046015),
        float(0.7345307424),
        float(0.9766246496),
        float(-0.02003735175),
        float(-0.4824156342),
        float(0.4245892007),
        float(0.9072427669),
        float(0.593346808),
        float(-0.8911762541),
        float(-0.7657571834),
        float(-0.5268198896),
        float(-0.8801903279),
        float(-0.6296409617),
        float(-0.09492481344),
        float(-0.4920470525),
        float(0.7307666154),
        float(-0.2514540636),
        float(-0.3356210347),
        float(-0.3522787894),
        float(0.87847885),
        float(-0.7424096346),
        float(0.5757585274),
        float(0.4519299338),
        float(0.6420368628),
        float(-0.1128478447),
        float(0.499874883),
        float(0.5291681739),
        float(-0.5098837195),
        float(0.5639583502),
        float(-0.8456386526),
        float(-0.9657134875),
        float(-0.576437342),
        float(-0.5666013014),
        float(0.5667702405),
        float(-0.481316582),
        float(0.7313389916),
        float(-0.3805628566),
        float(-0.6512675909),
        float(-0.2787156951),
        float(0.8648059114),
        float(-0.9730216276),
        float(-0.8335820906),
        float(0.2673159641),
        float(0.231150148),
        float(0.01286214638),
        float(0.6774953261),
        float(0.6542885718),
        float(-0.02545450161),
        float(0.2101238586),
        float(-0.5572105885),
        float(0.813705672),
        float(-0.7546026951),
        float(-0.2502500006),
        float(-0.9979289381),
        float(0.7024037039),
        float(0.08990874624),
        float(0.8170812432),
        float(0.4226980265),
        float(-0.2442153475),
        float(-0.9183326731),
        float(0.6068222411),
        float(0.818676691),
        float(-0.7236735282),
        float(-0.5383903295),
        float(-0.6269337242),
        float(-0.0939331121),
        float(0.9203878539),
        float(-0.7256396824),
        float(0.6292431149),
        float(0.4234156978),
        float(0.006685688024),
        float(-0.2598694113),
        float(0.6408036421),
        float(0.05899871622),
        float(0.7090281418),
        float(-0.5905222072),
        float(0.3128214264),
        float(-0.691925826),
        float(0.3634019349),
        float(-0.6772511147),
        float(-0.3204583896),
        float(-0.3906740409),
        float(-0.3342190395),
        float(-0.517779592),
        float(-0.6817711267),
        float(0.6422383105),
        float(0.4388482478),
        float(0.2968562611),
        float(-0.2019778353),
        float(0.6014865048),
        float(0.9519280722),
        float(0.3398889569),
        float(0.8179709354),
        float(0.2365522154),
        float(0.3262175096),
        float(-0.8060715954),
        float(-0.2068642503),
        float(0.6208057279),
        float(-0.5274282502),
        float(-0.3722334928),
        float(-0.8923412971),
        float(0.5341834201),
        float(-0.3663701513),
        float(-0.6114600319),
        float(0.5026307556),
        float(0.8396151729),
        float(0.9245042467),
        float(-0.7994843957),
        float(-0.5357200589),
        float(-0.6283359739),
        float(-0.61351886),
        float(-0.875632008),
        float(-0.5278879423),
        float(0.9087491985),
        float(-0.03500215466),
        float(-0.261365798),
        float(-0.579523541),
        float(-0.3765052689),
        float(-0.74398252),
        float(0.4257318052),
        float(-0.1214508921),
        float(0.8561809753),
        float(0.6802835104),
        float(-0.5452131039),
        float(-0.1997156478),
        float(0.4562348357),
        float(-0.811704301),
        float(0.67793962),
        float(-0.9237819106),
        float(0.6973511259),
        float(-0.5189506),
        float(0.5517320032),
        float(-0.396710831),
        float(0.5493762815),
        float(-0.2507853002),
        float(0.4788634005),
        float(0.387333516),
        float(-0.2176515694),
        float(0.6749832419),
        float(0.2148283022),
        float(-0.7521815872),
        float(0.4697000159),
        float(0.7890593699),
        float(-0.7606162952),
        float(0.01083397843),
        float(0.5254091908),
        float(-0.6748025877),
        float(0.751091524),
        float(0.05259056135),
        float(0.01889481232),
        float(-0.6037423727),
        float(-0.6542965129),
        float(0.08873301081),
        float(-0.6191345671),
        float(0.4331858488),
        float(-0.3858351946),
        float(-0.1429059747),
        float(0.4118221036),
        float(-0.6247153214),
        float(-0.611423014),
        float(0.5542939606),
        float(-0.9432768808),
        float(-0.4567870451),
        float(-0.7349133547),
        float(0.399304489),
        float(-0.7474927672),
        float(0.02589419753),
        float(0.783915821),
        float(0.6138668752),
        float(0.4276376047),
        float(-0.4347886353),
        float(0.02947841302),
        float(-0.833742746),
        float(0.3817221742),
        float(-0.8743368359),
        float(-0.3823443796),
        float(-0.6829243811),
        float(-0.3681903049),
        float(-0.367626833),
        float(-0.434583373),
        float(0.235891995),
        float(-0.6874880269),
        float(-0.5115661773),
        float(-0.5534962601),
        float(0.5632777056),
        float(0.686191532),
        float(-0.05095871588),
        float(-0.06865785057),
        float(-0.5975288531),
        float(-0.6429790056),
        float(-0.3729361548),
        float(0.2237917666),
        float(0.6046773225),
        float(-0.5041542295),
        float(-0.03972191174),
        float(0.7028828406),
        float(-0.5560856498),
        float(0.5898328456),
        float(-0.9308076766),
        float(0.4617069864),
        float(0.3190983137),
        float(0.9116567753),
        float(-0.45029554),
        float(0.3346334459),
        float(0.8525005645),
        float(0.2528483381),
        float(-0.8306630147),
        float(-0.6880390622),
        float(0.7448684026),
        float(-0.1963355843),
        float(-0.5900257974),
        float(0.9097057294),
        float(-0.2509196808),
};

const float CELL_3D_Y[] =
    {
        float(-0.6760585049),
        float(-0.09136176499),
        float(0.1681325679),
        float(-0.6688468686),
        float(-0.4822753902),
        float(-0.7891068824),
        float(-0.1877509944),
        float(0.548470914),
        float(-0.463339443),
        float(-0.4050542082),
        float(0.3218158513),
        float(0.2546493823),
        float(-0.3753271935),
        float(0.4745384887),
        float(0.481254652),
        float(-0.8934416489),
        float(-0.6737085076),
        float(0.7469917228),
        float(0.3826230411),
        float(0.6751013678),
        float(-0.7248119515),
        float(-0.3224276742),
        float(-0.02076190936),
        float(-0.6404268166),
        float(-0.5292028444),
        float(0.7151414636),
        float(-0.6144655059),
        float(-0.369912124),
        float(0.6942067212),
        float(-0.4481558248),
        float(-0.6366894559),
        float(0.5956568471),
        float(0.564274539),
        float(0.7145584688),
        float(0.6871918316),
        float(0.5657918509),
        float(-0.6275978114),
        float(0.4146983062),
        float(0.2638993789),
        float(-0.792633138),
        float(0.5706133514),
        float(0.8606546462),
        float(0.6490900316),
        float(-0.8242699196),
        float(0.6765819124),
        float(0.1959534069),
        float(-0.8426769757),
        float(-0.5917672797),
        float(0.7517364266),
        float(0.03252559226),
        float(0.0883617105),
        float(0.4475064813),
        float(-0.1418643552),
        float(0.7343428473),
        float(0.3870192548),
        float(-0.7716703522),
        float(0.4839898327),
        float(0.7437439055),
        float(-0.5989573348),
        float(-0.8357068955),
        float(0.6086049038),
        float(0.9194627258),
        float(0.4718297238),
        float(-0.2650335884),
        float(-0.6470352599),
        float(-0.5555181303),
        float(0.1222351235),
        float(0.7802044684),
        float(-0.8636947022),
        float(-0.2341352163),
        float(0.683030874),
        float(-0.5005858287),
        float(0.2334616211),
        float(0.2576877608),
        float(0.6666816727),
        float(-0.7663996863),
        float(0.794201982),
        float(0.6189308788),
        float(0.6071033261),
        float(-0.4206058253),
        float(-0.3957336915),
        float(-0.8170257484),
        float(-0.1043240417),
        float(0.0002167596213),
        float(0.1816339018),
        float(-0.6838094939),
        float(-0.2495341969),
        float(-0.7116756954),
        float(-0.03361673621),
        float(-0.3350836431),
        float(0.2137186039),
        float(0.2557996786),
        float(0.7490117093),
        float(0.4942936549),
        float(-0.352686853),
        float(-0.3952445435),
        float(-0.0459964767),
        float(-0.7115787471),
        float(0.08022899756),
        float(0.5362268157),
        float(-0.8258613686),
        float(0.1114171723),
        float(0.3882823051),
        float(-0.7915404457),
        float(0.3250957662),
        float(0.6401346464),
        float(-0.2662724517),
        float(-0.6727907114),
        float(-0.994730818),
        float(-0.3596358977),
        float(0.2344610069),
        float(-0.6645215546),
        float(-0.7107590611),
        float(-0.4646617327),
        float(0.6717191355),
        float(0.5101893498),
        float(0.1185768238),
        float(0.236005093),
        float(-0.7811024061),
        float(0.5089325193),
        float(0.6073187658),
        float(-0.7930732557),
        float(-0.6822767155),
        float(0.3201532885),
        float(0.7545302807),
        float(0.1072664448),
        float(0.6784033173),
        float(-0.6595924967),
        float(0.7276509498),
        float(0.5586689436),
        float(-0.6498636788),
        float(0.6789333174),
        float(0.7105966551),
        float(-0.2872214155),
        float(0.496746217),
        float(-0.3880337977),
        float(0.7324070604),
        float(-0.9326634749),
        float(-0.5867839255),
        float(0.8003043651),
        float(-0.1631882481),
        float(-0.6796374681),
        float(-0.8066678503),
        float(0.4238177418),
        float(0.7715863549),
        float(0.5455367347),
        float(-0.03205115397),
        float(-0.6005545066),
        float(-0.5423640002),
        float(0.3569205906),
        float(-0.582071752),
        float(0.6407354361),
        float(0.7777142984),
        float(-0.09956428618),
        float(0.1100002681),
        float(0.8136349123),
        float(0.2923431904),
        float(0.9735794425),
        float(0.8324974864),
        float(-0.6179617717),
        float(-0.9248386523),
        float(-0.6448780771),
        float(-0.5274402761),
        float(-0.7862170565),
        float(0.2682099744),
        float(-0.5848777694),
        float(-0.6364561467),
        float(-0.7167402514),
        float(-0.8677012494),
        float(0.4205286707),
        float(-0.7007832749),
        float(0.243272451),
        float(-0.1899846085),
        float(-0.6146124977),
        float(-0.8093357692),
        float(-0.03545096987),
        float(-0.7191590868),
        float(0.7478645848),
        float(0.3623517328),
        float(0.8436992512),
        float(-0.2445711729),
        float(0.6897356637),
        float(-0.1708070787),
        float(0.4639272368),
        float(-0.7917186656),
        float(0.02980025428),
        float(0.6334156172),
        float(-0.9815544807),
        float(-0.2307217304),
        float(0.1080823318),
        float(0.5167601798),
        float(-0.845120016),
        float(0.441572562),
        float(0.5876789172),
        float(-0.6365908737),
        float(0.68350166),
        float(0.5849723959),
        float(0.1164114357),
        float(-0.7379813884),
        float(-0.9613237178),
        float(-0.9071943084),
        float(-0.7682111105),
        float(0.639074459),
        float(-0.619358298),
        float(0.2807257131),
        float(-0.01800868791),
        float(0.3776607289),
        float(0.7207567823),
        float(0.5536661486),
        float(-0.9974053117),
        float(-0.02047200006),
        float(-0.6739453804),
        float(-0.5607471297),
        float(0.8815553192),
        float(0.8275977415),
        float(0.3928902456),
        float(0.550991396),
        float(0.4247623676),
        float(-0.3436948871),
        float(-0.3653537677),
        float(0.3181702902),
        float(-0.6067173171),
        float(-0.8984128477),
        float(0.4220839766),
        float(0.7238407199),
        float(-0.7766913695),
        float(0.6460037842),
        float(0.2544775664),
        float(0.6488840578),
        float(0.805016833),
        float(-0.9183807036),
        float(0.4144046357),
        float(0.270587208),
        float(-0.8813684494),
        float(0.6985971877),
        float(-0.7795603017),
        float(-0.8624480731),
        float(0.5532697017),
        float(0.711179521),
        float(-0.7798160574),
        float(0.5225859041),
        float(0.1261859368),
        float(0.3398033582),
        float(-0.7472173667),
        float(-0.4032647119),
        float(-0.4246578154),
        float(0.8481212377),
        float(-0.2144838537),
        float(0.3431714491),
        float(0.5310188231),
        float(0.6682978632),
        float(0.3110433206),
        float(0.9263293599),
        float(-0.6155600569),
        float(0.07169784399),
        float(0.8985888773),
};
const float CELL_3D_Z[] =
    {
        float(-0.6341391283),
        float(-0.7207118346),
        float(0.9597866014),
        float(0.3237504235),
        float(-0.7588642466),
        float(-0.01782410481),
        float(0.2515593809),
        float(0.2207257205),
        float(-0.8579541106),
        float(0.3406410681),
        float(0.7669470462),
        float(-0.9431957648),
        float(0.7676171537),
        float(-0.6000491115),
        float(-0.7062096948),
        float(0.2550207115),
        float(0.7347325213),
        float(0.5163625202),
        float(-0.5394270162),
        float(0.3336656285),
        float(-0.0638635111),
        float(-0.6503195787),
        float(0.3143356798),
        float(-0.5039217245),
        float(0.6605180464),
        float(-0.6855479011),
        float(-0.6693185756),
        float(0.1832083647),
        float(-0.5666258437),
        float(0.3576482138),
        float(-0.6571949095),
        float(-0.7522101635),
        float(-0.7238865886),
        float(0.4538887323),
        float(0.2467106257),
        float(0.7274778869),
        float(0.3151170655),
        float(-0.8802293764),
        float(-0.2167232729),
        float(0.5854637865),
        float(0.7019741052),
        float(0.5091756071),
        float(0.1973189533),
        float(0.46743546),
        float(0.05197599597),
        float(0.088354718),
        float(0.5380464843),
        float(-0.6458224544),
        float(-0.5045952393),
        float(0.419347884),
        float(0.8000823542),
        float(-0.07445020656),
        float(-0.6272881641),
        float(-0.428020311),
        float(-0.2747382083),
        float(-0.08987283726),
        float(0.8699098354),
        float(0.4524761885),
        float(-0.3274603257),
        float(0.4882262167),
        float(-0.7189983256),
        float(0.1746079907),
        float(0.0751772698),
        float(-0.6152927202),
        float(0.4998474673),
        float(-0.6979677227),
        float(0.7568667263),
        float(-0.6152612058),
        float(0.06447140991),
        float(-0.8155744872),
        float(-0.5229602449),
        float(0.6567836838),
        float(-0.4799905631),
        float(0.03153534591),
        float(0.4724992466),
        float(-0.3026458097),
        float(-0.2191225827),
        float(-0.620692287),
        float(0.3107552588),
        float(0.8235670294),
        float(0.6474915988),
        float(-0.5047637941),
        float(0.4911488878),
        float(-0.2307138167),
        float(-0.5216800015),
        float(0.6789305939),
        float(0.9403734863),
        float(0.702390397),
        float(0.7347584625),
        float(0.6779567958),
        float(0.9765635805),
        float(-0.9436177661),
        float(-0.358465925),
        float(-0.3058706624),
        float(0.5533414464),
        float(-0.8838306897),
        float(0.04496841812),
        float(0.01687374963),
        float(-0.9927133148),
        float(-0.211752318),
        float(0.3732015249),
        float(0.9632990593),
        float(-0.07682417004),
        float(-0.07232213047),
        float(0.4733721775),
        float(0.2579229713),
        float(0.7995216286),
        float(0.3928189967),
        float(0.04107517667),
        float(0.1534542912),
        float(0.6468965045),
        float(0.4030684878),
        float(-0.5617300988),
        float(-0.885463029),
        float(0.693729985),
        float(-0.5736527866),
        float(-0.9911905409),
        float(-0.66451538),
        float(0.2028855685),
        float(0.8019541421),
        float(-0.3903877149),
        float(-0.4888495114),
        float(-0.2753714057),
        float(-0.8915202143),
        float(0.5273119089),
        float(0.9363714773),
        float(-0.5212228249),
        float(-0.31642672),
        float(0.2409440761),
        float(-0.703776404),
        float(-0.6996810411),
        float(-0.7058714505),
        float(-0.3650566783),
        float(0.1064744278),
        float(0.7985729102),
        float(0.424680257),
        float(-0.6384535592),
        float(0.1540161646),
        float(-0.07702731943),
        float(-0.5627789132),
        float(-0.7667919169),
        float(-0.509815999),
        float(0.4590525092),
        float(0.1552595611),
        float(0.345402042),
        float(0.7537656024),
        float(0.7906259247),
        float(-0.6218493452),
        float(0.02979350071),
        float(-0.1337893489),
        float(-0.1483818606),
        float(0.549965562),
        float(0.01882482408),
        float(-0.7833783002),
        float(0.4702855809),
        float(0.2435827372),
        float(0.2978428332),
        float(0.2256499906),
        float(0.4885036897),
        float(0.5312962584),
        float(0.05401156992),
        float(0.1749922158),
        float(-0.7352273018),
        float(0.6058980284),
        float(0.4416079111),
        float(0.4417378638),
        float(0.5455879807),
        float(-0.6681295324),
        float(0.1973431441),
        float(0.4053292055),
        float(0.2220375492),
        float(0.2957118467),
        float(0.6910913512),
        float(0.5940890106),
        float(-0.2014135283),
        float(-0.9172588213),
        float(-0.4254361401),
        float(-0.6146586825),
        float(-0.7996193253),
        float(-0.3716777111),
        float(-0.9448876842),
        float(-0.2620349924),
        float(0.9615995749),
        float(-0.4679683524),
        float(0.3905937144),
        float(0.613593722),
        float(0.1422937358),
        float(0.1908754211),
        float(0.8189704912),
        float(-0.7300408736),
        float(-0.4108776451),
        float(-0.5319834504),
        float(-0.8970265651),
        float(-0.5386359045),
        float(0.4082255906),
        float(0.7245356676),
        float(0.5239080873),
        float(-0.8937552226),
        float(-0.553637673),
        float(0.2354455182),
        float(-0.0860293075),
        float(-0.1399373318),
        float(-0.4666323327),
        float(0.5560157407),
        float(0.1772619533),
        float(-0.8893937725),
        float(-0.5632714576),
        float(-0.5666264959),
        float(-0.3670263736),
        float(-0.06717242579),
        float(0.6205295181),
        float(-0.4110536264),
        float(0.7090054553),
        float(0.183899597),
        float(-0.5605470555),
        float(0.3879565548),
        float(0.7420893903),
        float(-0.2347595118),
        float(-0.8577217497),
        float(0.6325590203),
        float(-0.8736152276),
        float(0.7048011129),
        float(-0.06317948268),
        float(0.8753285574),
        float(-0.05843650473),
        float(-0.3674922622),
        float(-0.5256624401),
        float(0.7861039337),
        float(0.3287714416),
        float(0.5910593099),
        float(-0.3896960134),
        float(0.6864605361),
        float(0.7164918431),
        float(-0.290014277),
        float(-0.6796169617),
        float(0.1632515592),
        float(0.04485347486),
        float(0.8320545697),
        float(0.01339408056),
        float(-0.2874989857),
        float(0.615630723),
        float(0.3430367014),
        float(0.8193658136),
        float(-0.5829600957),
        float(0.07911697781),
        float(0.7854296063),
        float(-0.4107442306),
        float(0.4766964066),
        float(-0.9045999527),
        float(-0.1673856787),
        float(0.2828077348),
        float(-0.5902737632),
        float(-0.321506229),
        float(-0.5224513133),
        float(-0.4090169985),
        float(-0.3599685311),
};

const double GRAD_X[] =
    {
        1, -1, 1, -1,
        1, -1, 1, -1,
        0, 0, 0, 0};
const double GRAD_Y[] =
    {
        1, 1, -1, -1,
        0, 0, 0, 0,
        1, -1, 1, -1};
const double GRAD_Z[] =
    {
        0, 0, 0, 0,
        1, 1, -1, -1,
        1, 1, -1, -1};

Noiser::Noiser(void)
{
    noise_2d_multiplier = NOISE_2D_MULTIPLIER;
    noise_2d_divider = NOISE_2D_DIVIDER;
    noise_2d_persistence = NOISE_2D_PERSISTENCE;
    noise_2d_octaves = NOISE_2D_OCTAVES;
    noise_2d_strength = NOISE_2D_STRENGTH;
    noise_2d_scale = NOISE_2D_SCALE;

    noise_3d_simplex_divider = NOISE_3D_SIMPLEX_DIVIDER;
    noise_3d_simplex_persistence = NOISE_3D_SIMPLEX_PERSISTENCE;
    noise_3d_simplex_octaves = NOISE_3D_SIMPLEX_OCTAVES;
    noise_3d_simplex_strength = NOISE_3D_SIMPLEX_STRENGTH;
    noise_3d_simplex_scale = NOISE_3D_SIMPLEX_SCALE;

    noise_3d_perlin_divider = NOISE_3D_PERLIN_DIVIDER;
    noise_3d_perlin_persistence = NOISE_3D_PERLIN_PERSISTENCE;
    noise_3d_perlin_octaves = NOISE_3D_PERLIN_OCTAVES;
    noise_3d_perlin_strength = NOISE_3D_PERLIN_STRENGTH;
    noise_3d_perlin_scale = NOISE_3D_PERLIN_SCALE;

    noise_3d_cell_divider = NOISE_3D_CELL_DIVIDER;
    noise_3d_cell_frequency = NOISE_3D_CELL_FREQUENCY;
    noise_3d_cell_jitter = NOISE_3D_CELL_JITTER;
    noise_3d_cell_map_low = NOISE_3D_CELL_MAP_LOW;
    noise_3d_cell_map_max = NOISE_3D_CELL_MAP_MAX;
    SQRT3 = float(1.7320508075688772935274463415059);
    F2 = float(0.5) * (SQRT3 - float(1.0));
    G2 = (float(3.0) - SQRT3) / float(6.0);
    F3 = 1 / float(3);
    G3 = 1 / float(6);
    return;
}

void Noiser::set_seed(int c_seed)
{
    this->seed = c_seed;
}

float Noiser::noise_simplex(unsigned char offset, float x, float y)
{
    float t = (x + y) * F2;
    int i = floor(x + t);
    int j = floor(y + t);

    t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;

    float x0 = x - X0;
    float y0 = y - Y0;

    int i1, j1;
    if (x0 > y0)
    {
        i1 = 1;
        j1 = 0;
    }
    else
    {
        i1 = 0;
        j1 = 1;
    }

    float x1 = x0 - (float)i1 + G2;
    float y1 = y0 - (float)j1 + G2;
    float x2 = x0 - 1 + 2 * G2;
    float y2 = y0 - 1 + 2 * G2;

    float n0, n1, n2;

    t = float(0.5) - x0 * x0 - y0 * y0;
    if (t < 0)
        n0 = 0;
    else
    {
        t *= t;
        n0 = t * t * gradCoord2D(offset, i, j, x0, y0);
    }

    t = float(0.5) - x1 * x1 - y1 * y1;
    if (t < 0)
        n1 = 0;
    else
    {
        t *= t;
        n1 = t * t * gradCoord2D(offset, i + i1, j + j1, x1, y1);
    }

    t = float(0.5) - x2 * x2 - y2 * y2;
    if (t < 0)
        n2 = 0;
    else
    {
        t *= t;
        n2 = t * t * gradCoord2D(offset, i + 1, j + 1, x2, y2);
    }

    return 70 * (n0 + n1 + n2);
}

float Noiser::lerp(float a, float b, float c)
{
    return a + c * (b - a);
}

float Noiser::noise_perlin(unsigned char offset, float x, float y, float z) 
{
	int x0 = floor(x);
	int y0 = floor(y);
	int z0 = floor(z);
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int z1 = z0 + 1;

	float xs, ys, zs;
	xs = x - (float)x0;
	ys = y - (float)y0;
	zs = z - (float)z0;

	float xd0 = x - (float)x0;
	float yd0 = y - (float)y0;
	float zd0 = z - (float)z0;
	float xd1 = xd0 - 1;
	float yd1 = yd0 - 1;
	float zd1 = zd0 - 1;

	float xf00 = lerp(gradCoord3D(offset, x0, y0, z0, xd0, yd0, zd0), gradCoord3D(offset, x1, y0, z0, xd1, yd0, zd0), xs);
	float xf10 = lerp(gradCoord3D(offset, x0, y1, z0, xd0, yd1, zd0), gradCoord3D(offset, x1, y1, z0, xd1, yd1, zd0), xs);
	float xf01 = lerp(gradCoord3D(offset, x0, y0, z1, xd0, yd0, zd1), gradCoord3D(offset, x1, y0, z1, xd1, yd0, zd1), xs);
	float xf11 = lerp(gradCoord3D(offset, x0, y1, z1, xd0, yd1, zd1), gradCoord3D(offset, x1, y1, z1, xd1, yd1, zd1), xs);

	float yf0 = lerp(xf00, xf10, ys);
	float yf1 = lerp(xf01, xf11, ys);

	return lerp(yf0, yf1, zs);  
}

float Noiser::noise_simplex(unsigned char offset, float x, float y, float z)
{
    float t = (x + y + z) * F3;
    int i = floor(x + t);
    int j = floor(y + t);
    int k = floor(z + t);

    t = (i + j + k) * G3;
    float X0 = i - t;
    float Y0 = j - t;
    float Z0 = k - t;

    float x0 = x - X0;
    float y0 = y - Y0;
    float z0 = z - Z0;

    int i1, j1, k1;
    int i2, j2, k2;

    if (x0 >= y0)
    {
        if (y0 >= z0)
        {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0;
        }
        else if (x0 >= z0)
        {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 0;
            k2 = 1;
        }
        else // x0 < z0
        {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 1;
            j2 = 0;
            k2 = 1;
        }
    }
    else // x0 < y0
    {
        if (y0 < z0)
        {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 0;
            j2 = 1;
            k2 = 1;
        }
        else if (x0 < z0)
        {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 0;
            j2 = 1;
            k2 = 1;
        }
        else // x0 >= z0
        {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0;
        }
    }

    float x1 = x0 - i1 + G3;
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2 * G3;
    float y2 = y0 - j2 + 2 * G3;
    float z2 = z0 - k2 + 2 * G3;
    float x3 = x0 - 1 + 3 * G3;
    float y3 = y0 - 1 + 3 * G3;
    float z3 = z0 - 1 + 3 * G3;

    float n0, n1, n2, n3;

    t = float(0.6) - x0 * x0 - y0 * y0 - z0 * z0;
    if (t < 0)
        n0 = 0;
    else
    {
        t *= t;
        n0 = t * t * gradCoord3D(offset, i, j, k, x0, y0, z0);
    }

    t = float(0.6) - x1 * x1 - y1 * y1 - z1 * z1;
    if (t < 0)
        n1 = 0;
    else
    {
        t *= t;
        n1 = t * t * gradCoord3D(offset, i + i1, j + j1, k + k1, x1, y1, z1);
    }

    t = float(0.6) - x2 * x2 - y2 * y2 - z2 * z2;
    if (t < 0)
        n2 = 0;
    else
    {
        t *= t;
        n2 = t * t * gradCoord3D(offset, i + i2, j + j2, k + k2, x2, y2, z2);
    }

    t = float(0.6) - x3 * x3 - y3 * y3 - z3 * z3;
    if (t < 0)
        n3 = 0;
    else
    {
        t *= t;
        n3 = t * t * gradCoord3D(offset, i + 1, j + 1, k + 1, x3, y3, z3);
    }

    return 32 * (n0 + n1 + n2 + n3);
}

float Noiser::gradCoord2D(unsigned char offset, int x, int y, float xd, float yd)
{
    unsigned char lutPos = Index2D_12(offset, x, y);

    return xd * GRAD_X[lutPos] + yd * GRAD_Y[lutPos];
}

float Noiser::gradCoord3D(unsigned char offset, int x, int y, int z, float xd, float yd, float zd)
{
    unsigned char lutPos = Index3D_12(offset, x, y, z);

    return xd * GRAD_X[lutPos] + yd * GRAD_Y[lutPos] + zd * GRAD_Z[lutPos];
}

unsigned char Noiser::Index2D_12(unsigned char offset, int x, int y)
{
    return m_perm12[(x & 0xff) + m_perm[(y & 0xff) + offset]];
}

unsigned char Noiser::Index3D_12(unsigned char offset, int x, int y, int z)
{
    return m_perm12[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + offset]]];
}

void Noiser::create_seed(void)
{
    std::mt19937_64 gen(seed);

    for (int i = 0; i < 256; i++)
        m_perm[i] = i;

    for (int j = 0; j < 256; j++)
    {
        int rng = (int)(gen() % (256 - j));
        int k = rng + j;
        int l = m_perm[j];
        m_perm[j] = m_perm[j + 256] = m_perm[k];
        m_perm[k] = l;
        m_perm12[j] = m_perm12[j + 256] = m_perm[j] % 12;
    }
}

float Noiser::noise2d(float x, float y, int octaves, float persistence)
{
    float sum = 0;
    float strength = noise_2d_strength;
    float scale = noise_2d_scale;

    // unsigned char rng = m_perm12[int(x+y)];
    for (int i = 0; i < octaves; i++)
    {
        // sum += strength * glm::simplex(glm::vec2(x, y) * scale);
        glm::vec2 tmp = glm::vec2(x,y) * scale;
        sum += strength * noise_simplex(0, tmp.x, tmp.y);
        scale *= 2.0;
        strength *= persistence;
    }

    return sum;
}

float Noiser::noise3d_abs(float x, float y, float z, int octaves, float persistence)
{
    float sum = 0;
    float strength = noise_3d_simplex_strength;
    float scale = noise_3d_simplex_scale;

    // unsigned char rng = m_perm12[int(x+y+z)];
    for (int i = 0; i < octaves; i++)
    {
        // sum += strength * fabs(glm::simplex(glm::vec3(x,y,z) * scale));
        glm::vec3 tmp = glm::vec3(x,y,z) * scale;
        sum += strength * fabs(noise_simplex(0, tmp.x, tmp.y, tmp.z));
        scale *= 2.0;
        strength *= persistence;
    }

    return sum;
}

float Noiser::noise_3d_perlin(float x, float y, float z, int octaves, float persistence)
{
    float sum = 0;
    float strength = noise_3d_perlin_strength;
    float scale = noise_3d_perlin_scale;

    for (int i = 0; i < octaves; i++)
    {
        // sum += strength * fabs(glm::perlin(glm::vec3(x, y, z) * scale));
        glm::vec3 tmp = glm::vec3(x,y,z) * scale;
        sum += strength * fabs(noise_perlin(0, tmp.x, tmp.y, tmp.z));
        scale *= 2.0;
        strength *= persistence;
    }

    return sum;
}

float Noiser::cellular_noise(float x, float y, float z, float frequency)
{
    x *= frequency;
    y *= frequency;
    z *= frequency;
    return cellular(x, y, z, noise_3d_cell_map_low, noise_3d_cell_map_max);
}

unsigned char Noiser::Index3D_256(unsigned char offset, int x, int y, int z)
{
    return m_perm[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + offset]]];
}

float Noiser::cellular(float x, float y, float z, int cellDist0, int cellDist1)
{
    float jitter = noise_3d_cell_jitter;
    int xr = round(x);
    int yr = round(y);
    int zr = round(z);

    float distance[3 + 1] = {999999, 999999, 999999, 999999};

    for (int xi = xr - 1; xi <= xr + 1; xi++)
    {
        for (int yi = yr - 1; yi <= yr + 1; yi++)
        {
            for (int zi = zr - 1; zi <= zr + 1; zi++)
            {
                unsigned char lutPos = Index3D_256(0, xi, yi, zi);

                float vecX = xi - x + CELL_3D_X[lutPos] * jitter;
                float vecY = yi - y + CELL_3D_Y[lutPos] * jitter;
                float vecZ = zi - z + CELL_3D_Z[lutPos] * jitter;

                float newDistance = vecX * vecX + vecY * vecY + vecZ * vecZ;

                for (int i = cellDist1; i > 0; i--)
                    distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
                distance[0] = fmin(distance[0], newDistance);
            }
        }
    }

    return distance[cellDist0] / distance[cellDist1];
}

Noiser::~Noiser(void)
{
    return;
}
