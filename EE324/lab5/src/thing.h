
float sin_lut[] = {
0.0,
0.009999833334166664,
0.01999866669333308,
0.02999550020249566,
0.03998933418663416,
0.04997916927067833,
0.0599640064794446,
0.06994284733753277,
0.0799146939691727,
0.08987854919801104,
0.09983341664682814,
0.1097783008371748,
0.11971220728891935,
0.12963414261969483,
0.13954311464423647,
0.14943813247359922,
0.15931820661424598,
0.16918234906699603,
0.1790295734258242,
0.1888588949765006,
0.19866933079506124,
0.2084598998460996,
0.21822962308086938,
0.22797752353518846,
0.23770262642713466,
0.247403959254523,
0.2570805518921552,
0.2667314366888312,
0.2763556485641138,
0.28595222510483564,
0.29552020666133966,
0.3050586364434436,
0.3145665606161179,
0.3240430283948685,
0.3334870921408145,
0.3428978074554515,
0.3522742332750901,
0.36161543196496215,
0.37092046941298285,
0.3801884151231616,
0.3894183423086507,
0.39860932798442306,
0.40776045305957037,
0.416870802429211,
0.42593946506599983,
0.4349655341112304,
0.44394810696552,
0.4528862853790685,
0.4617791755414831,
0.4706258881711583,
0.47942553860420317,
0.4881772468829077,
0.4968801378437369,
0.5055333412048472,
0.5141359916531133,
0.5226872289306594,
0.5311861979208836,
0.5396320487339695,
0.5480239367918738,
0.556361022912784,
0.5646424733950356,
0.5728674601004815,
0.5810351605373053,
0.5891447579422697,
0.5971954413623923,
0.6051864057360399,
0.613116851973434,
0.6209859870365599,
0.6287930240184688,
0.6365371822219682,
0.6442176872376913,
0.651833771021537,
0.6593846719714734,
0.6668696350036982,
0.6742879116281454,
0.6816387600233345,
0.6889214451105516,
0.696135238627357,
0.7032794192004105,
0.7103532724176082,
0.7173560908995231,
0.7242871743701429,
0.7311458297268962,
0.7379313711099631,
0.7446431199708596,
0.751280405140293,
0.7578425628952773,
0.7643289370255054,
0.7707388788989696,
0.7770717475268242,
0.7833269096274837,
0.7895037396899508,
0.7956016200363664,
0.8016199408837775,
0.8075581004051147,
0.8134155047893741,
0.8191915683009986,
0.8248857133384504,
0.8304973704919708,
0.8360259786005209,
0.8414709848078968,
0.8468318446180155,
0.8521080219493633,
0.8572989891886037,
0.8624042272433388,
0.8674232255940173,
0.8723554823449866,
0.877200504274682,
0.8819578068849478,
0.8866269144494876,
0.8912073600614356,
0.895698685680048,
0.9001004421765053,
0.9044121893788263,
0.9086334961158836,
0.9127639402605214,
0.9168031087717673,
0.920750597736136,
0.9246060124080206,
0.928368967249167,
0.9320390859672266,
0.9356160015533862,
0.9390993563190678,
0.9424888019316978,
0.9457839994495393,
0.9489846193555865,
0.9520903415905161,
0.9551008555846925,
0.9580158602892253,
0.9608350642060729,
0.9635581854171932,
0.9661849516127343,
0.9687151001182654,
0.9711483779210448,
0.9734845416953196,
0.9757233578266593,
0.9778646024353164,
0.9799080613986144,
0.9818535303723599,
0.9837008148112767,
0.9854497299884604,
0.9871001010138505,
0.98865176285172,
0.990104560337178,
0.9914583481916865,
0.9927129910375886,
0.993868363411645,
0.994924349777581,
0.9958808445376401,
0.9967377520431435,
0.9974949866040546,
0.9981524724975482,
0.9987101439755831,
0.999167945271476,
0.9995258306054791,
0.999783764189357,
0.9999417202299663,
0.9999996829318346,
0.9999576464987401,
0.9998156151342908,
0.9995736030415051,
0.9992316344213905,
0.998789743470524,
0.9982479743776324,
0.9976063813191736,
0.9968650284539188,
0.9960239899165366,
0.9950833498101801,
0.9940432021980758,
0.9929036510941184,
0.9916648104524685,
0.9903268041561579,
0.9888897660047012,
0.9873538397007162,
0.9857191788355533,
0.9839859468739367,
0.9821543171376182,
0.9802244727880453,
0.9781966068080444,
0.9760709219825239,
0.9738476308781949,
0.971526955822315,
0.9691091288804561,
0.9665943918332972,
0.9639829961524478,
0.9612752029752996,
0.9584712830789138,
0.9555715168529435,
0.9525761942715949,
0.94948561486463,
0.946300087687414,
0.94301993129001,
0.9396454736853244,
0.9361770523163055,
0.9326150140221999,
0.9289597150038688,
0.9252115207881677,
0.9213708061913948,
0.9174379552818093,
0.9134133613412245,
0.9092974268256812,
0.9050905633252004,
0.9007931915226269,
0.8964057411515596,
0.8919286509533794,
0.8873623686333753,
0.8827073508159741,
0.8779640629990781,
0.8731329795075167,
0.8682145834456129,
0.8632093666488742,
0.8581178296348094,
0.8529404815528769,
0.8476778401335705,
0.8423304316366467,
0.8368987907984987,
0.8313834607786843,
0.8257849931056094,
0.8201039476213756,
0.8143408924257974,
0.8084964038195919,
0.8025710662467491,
0.7965654722360886,
0.790480222342007,
0.7843159250844224,
0.7780731968879238,
0.7717526620201285,
0.7653549525292563,
0.758880708180925,
0.7523305763941739,
0.7457052121767236,
0.7390052780594745,
0.7322314440302551,
0.7253843874668235,
0.7184647930691302,
0.7114733527908488,
0.7044107657701806,
0.6972777382599425,
0.6900749835569413,
0.6828032219306449,
0.6754631805511562,
0.6680555934164966,
0.6605812012792064,
0.6530407515722708,
0.6454349983343768,
0.6377647021345101,
0.6300306299958988,
0.6222335553193116,
0.6143742578057187,
0.6064535233783221,
0.598472144103964,
0.5904309181139206,
0.5823306495240899,
0.5741721483545806,
0.5659562304487111,
0.5576837173914255,
0.5493554364271356,
0.5409722203769975,
0.5325349075556305,
0.5240443416872855,
0.515501371821474,
0.5069068522480634,
0.49826164241184895,
0.48956660682660996,
0.48082261498865914,
0.47203054128989363,
0.46319126493035656,
0.45430566983031795,
0.4453746445418831,
0.4363990821601383,
0.4273798802338422,
0.41831794067567146,
0.4092141696720303,
0.40006947759243255,
0.3908847788984657,
0.3816609920523452,
0.3723990394250693,
0.36309984720418237,
0.35376434530115725,
0.3443934672584046,
0.3349881501559197,
0.3255493345175751,
0.31607796421706896,
0.3065749863835386,
0.2970413513068481,
0.28747801234256054,
0.277885925816603,
0.2682660509296346,
0.25861934966112754,
0.24894678667316977,
0.23924932921399966,
0.22952794702128193,
0.2197836122251347,
0.2100172992509174,
0.2002299847217888,
0.1904226473610458,
0.1805962678942517,
0.17075182895116459,
0.16089031496747505,
0.1510127120863636,
0.141120008059887,
0.13121319215020405,
0.12129325503065003,
0.11136118868667032,
0.10141798631662263,
0.09146464223245798,
0.08150215176029037,
0.07153151114086496,
0.061553717429934866,
0.05156976839855636,
0.041580662433312675,
0.031587398436476094,
0.0215909757261186,
0.011592393936180922,
0.0015926529165099209,
-0.008407247367125526,
-0.018406306933030275,
-0.02840352588358026,
-0.03839790450521142,
-0.0483884433683902,
-0.0583741434275557,
-0.06835400612102341,
-0.07832703347084051,
-0.08829222818258282,
-0.09824859374508349,
-0.10819513453008321,
-0.11813085589179223,
-0.12805476426635412,
-0.13796586727120133,
-0.1478631738042926,
-0.15774569414322234,
-0.16761244004419204,
-0.1774624248408339,
-0.18729466354287658,
-0.1971081729346433,
-0.20690197167337282,
-0.21667508038735275,
-0.2264265217738559,
-0.2361553206968698,
-0.24586050428460948,
-0.25554110202680375,
-0.26519614587174556,
-0.27482467032309627,
-0.28442571253643445,
-0.2939983124155396,
-0.30354151270840096,
-0.31305435910294194,
-0.3225359003224504,
-0.3319851882207056,
-0.34140127787679214,
-0.35078322768959114,
-0.3601300994719396,
-0.3694409585444482,
-0.37871487382896885,
-0.3879509179417012,
-0.39714816728593083,
-0.40630570214438755,
-0.4154226067712168,
-0.42449796948355323,
-0.4335308827526885,
-0.442520443294823,
-0.45146575216139384,
-0.4603659148289687,
-0.4692200412886977,
-0.4780272461353132,
-0.4867866486556699,
-0.4954973729168152,
-0.5041585478535819,
-0.512769307355694,
-0.521328790354377,
-0.5298361409084636,
-0.538290508289988,
-0.5466910470692574,
-0.5550369171993943,
-0.5633272841003404,
-0.5715613187423143,
-0.5797381977287135,
-0.5878571033784533,
-0.5959172238077346,
-0.6039177530112312,
-0.6118578909426897,
-0.619736843594934,
-0.6275538230792643,
-0.6353080477042465,
-0.6429987420538799,
-0.6506251370651384,
-0.6581864701048762,
-0.6656819850460903,
-0.6731109323435331,
-0.6804725691086654,
-0.6877661591839453,
-0.6949909732164435,
-0.7021462887307773,
-0.709231390201358,
-0.7162455691239427,
-0.7231881240864843,
-0.7300583608392719,
-0.7368555923643557,
-0.7435791389442472,
-0.7502283282298917,
-0.7568024953079012,
-0.7633009827670469,
-0.7697231407639977,
-0.776068327088306,
-0.7823359072266267,
-0.7885252544261693,
-0.7946357497573715,
-0.8006667821757921,
-0.8066177485832153,
-0.8124880538879594,
-0.8182771110643858,
-0.8239843412116011,
-0.8296091736113466,
-0.8351510457850696,
-0.840609403550171,
-0.8459837010754228,
-0.8512734009355513,
-0.8564779741649782,
-0.861596900310718,
-0.8666296674844217,
-0.871575772413566,
-0.8764347204917796,
-0.8812060258283039,
-0.8858892112965813,
-0.8904838085819675,
-0.894989358228563,
-0.8994054096851575,
-0.9037315213502856,
-0.9079672606163857,
-0.912112203913061,
-0.916165936749436,
-0.9201280537556052,
-0.9239981587231696,
-0.9277758646448576,
-0.9314607937532251,
-0.9350525775584321,
-0.938550856885091,
-0.9419552819081846,
-0.9452655121880474,
-0.9484812167044101,
-0.9516020738895008,
-0.9546277716602015,
-0.9575580074492568,
-0.9603924882355295,
-0.963130930573303,
-0.9657730606206257,
-0.9683186141666945,
-0.9707673366582761,
-0.973118983225162,
-0.9753733187046552,
-0.9775301176650862,
-0.9795891644283565,
-0.9815502530915056,
-0.9834131875473012,
-0.9851777815038505,
-0.9868438585032281,
-0.9884112519391225,
-0.9898798050734964,
-0.9912493710522599,
-0.9925198129199566,
-0.9936910036334584,
-0.99476282607467,
-0.9957351730622404,
-0.9966079473622811,
-0.9973810616980894,
-0.998054438758876,
-0.998628011207496,
-0.9991017216871825,
-0.9994755228272822,
-0.9997493772479927,
-0.9999232575641002,
-0.9999971463877179,
-0.9999710363300249,
-0.9998449300020054,
-0.999618840014187,
-0.9992927889753801,
-0.9988668094904168,
-0.9983409441568909,
-0.9977152455608972,
-0.996989776271774,
-0.9961646088358458,
-0.9952398257691683,
-0.9942155195492777,
-0.9930917926059423,
-0.9918687573109201,
-0.9905465359667213,
-0.9891252607943786,
-0.9876050739202247,
-0.9859861273616802,
-0.984268583012052,
-0.9824526126243437,
-0.9805383977940807,
-0.978526129941151,
-0.9764160102906628,
-0.9742082498528228,
-0.9719030694018352,
-0.9695006994538239,
-0.9670013802437817,
-0.9644053617015469,
-0.9617129034268105,
-0.9589242746631561,
-0.9560397542711363,
-0.9530596307003866,
-0.9499842019607805,
-0.9468137755926291,
-0.9435486686359275,
-0.9401892075986501,
-0.9367357284241014,
-0.9331885764573206,
-0.9295481064105487,
-0.9258146823277567,
-0.9219886775482414,
-0.9180704746692927,
-0.9140604655079333,
-0.9099590510617376,
-0.9057666414687323,
-0.9014836559663832,
-0.8971105228496714,
-0.8926476794282643,
-0.8880955719827849,
-0.8834546557201844,
-0.8787253947282216,
-0.8739082619290548,
-0.8690037390319495,
-0.8640123164851083,
-0.8589344934266266,
-0.8537707776345785,
-0.8485216854762403,
-0.8431877418564535,
-0.8377694801651351,
-0.8322674422239392,
-0.8266821782320747,
-0.8210142467112865,
-0.8152642144500035,
-0.8094326564466602,
-0.803520155852197,
-0.7975273039117466,
-0.7914546999055089,
-0.785302951088824,
-0.7790726726314472,
-0.7727644875560322,
-0.76637902667583,
-0.7599169285316072,
-0.7533788393277933,
-0.7467654128678601,
-0.7400773104889428,
-0.7333152009957055,
-0.7264797605934625,
-0.719571672820558,
-0.7125916284800126,
-0.7055403255704435,
-0.6984184692162657,
-0.6912267715971797,
-0.6839659518769544,
-0.6766367361315111,
-0.6692398572763167,
-0.6617760549930929,
-0.6542460756558478,
-0.6466506722562404,
-0.6389906043282811,
-0.6312666378723795,
-0.6234795452787443,
-0.6156301052501458,
-0.6077191027240457,
-0.5997473287941042,
-0.591715580631071,
-0.5836246614030693,
-0.5754753801952796,
-0.5672685519290316,
-0.5590049972803128,
-0.5506855425977021,
-0.5423110198197346,
-0.5338822663917095,
-0.5254001251819458,
-0.5168654443974957,
-0.5082790774993257,
-0.49964188311697016,
-0.49095472496266984,
-0.4822184717450008,
-0.4734339970820047,
-0.46460217941382737,
-0.4557239019148759,
-0.4468000524055015,
-0.4378315232632188,
-0.4288192113334681,
-0.4197640178399322,
-0.4106668482944145,
-0.4015286124062887,
-0.3923502239915282,
-0.383132600881326,
-0.3738766648303118,
-0.3645833414243774,
-0.355253559988119,
-0.3458882534919055,
-0.336488358458582,
-0.3270548148698187,
-0.31758856607211317,
-0.3080905586824567,
-0.2985617424936733,
-0.28900307037944106,
-0.279415498199006,
-0.26979998470159655,
-0.2601574914305495,
-0.2504889826271566,
-0.24079542513424107,
-0.23107778829947434,
-0.2213370438784418,
-0.2115741659374684,
-0.2017901307562124,
-0.1919859167300387,
-0.18216250427217973,
-0.17232087571569513,
-0.16246201521523917,
-0.1525869086486463,
-0.14269654351834388,
-0.132791908852603,
-0.12287399510663644,
-0.11294379406355384,
-0.10300229873518443,
-0.09305050326277645,
-0.08308940281758402,
-0.07311999350135077,
-0.06314327224670052,
-0.05316023671744482,
-0.043171885208817415,
-0.033179216547645586,
-0.023183229992468246,
-0.01318492513361095,
-0.003185301793227696
};

float lsin(float val) {
	int idx = (int)((val * 100)) % (sizeof(sin_lut) / sizeof(float));
	if (idx < 0)
		idx = -idx;
	return sin_lut[idx];
}
