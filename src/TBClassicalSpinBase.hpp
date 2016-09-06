/*-----------------------------------------------------------|
| Copyright (C) 2016 Yuan-Yen Tai, Hongchul Choi,            |
|                    Jian-Xin Zhu                            |
|                                                            |
| This file is distributed under the terms of the GNU        |
| General Public License. See the file `LICENSE' in          |
| the root directory of the present distribution, or         |
| http://www.gnu.org/copyleft/gpl.txt .                      |
|                                                            |
|-----------------------------------------------------------*/

//
//  TBClassicalSpinBase.hpp
//  TBM^3
//

class TBClassicalSpinBase{
public:
	TBClassicalSpinBase(TBDataSource & _tbd): TBD(_tbd)		{ }
	
	void addHundCoupling	(string opt, string svar)		{
		replaceAll(opt, "\t", " ");
		auto  parser = split(opt, " ");
		if( parser.size() != 2){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		
		auto atomI = TBD.Lat.getAtom();
		
		removeSpace(svar);
		auto svarParser = split(svar, "*");
		auto orderKey = svarParser[0];
		auto forceKey = "@:"+parser[1]+":4den";
		r_var Jh = 1.0;
		if( svarParser.size() >= 2){
			for( unsigned i=1 ; i<svarParser.size() ; i++){
				Jh = Jh * TBD.parseSiteString(atomI, svarParser[i])[0].real();
			}
		}
		
		auto ordS = TBD.order.findOrder(atomI, orderKey);
		if( !ordS.first )	return;
		
		auto forceS = TBD.order.findOrder(atomI, forceKey);
		if( !forceS.first )	return;
		
		x_mat pspin(1,3);
		pspin[0] = forceS.second[1];
		pspin[1] = forceS.second[2];
		pspin[2] = forceS.second[3];
		
		HundCouplingList.push_back(boost::make_tuple(atomI, pspin, Jh, orderKey));
	}
	void addSuperExchange	(string opt, string svar)		{ // svar === " @:cspin * Jse "
		replaceAll(opt, "\t", " ");
		auto  dummyParser = split(opt, " ");
		if( dummyParser.size() != 1){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		
		removeSpace(opt);
		auto parser = split(opt, ":");
		if( parser.size() != 3){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		
		auto pair = TBD.Lat.getPair(parser[2]);
		
		if( pair.atomI.atomName != parser[0])	return;
		if( pair.atomJ.atomName != parser[1])	return;
		if( !pair.withinRange() )				return;
		
		removeSpace(svar);
		auto svarParser = split(svar, "*");
		auto orderKey = svarParser[0];
		r_var Jsx = 1.0;
		if( svarParser.size() == 2) Jsx = TBD.parseSiteString(pair.atomI, svarParser[1])[0].real();
		
		auto ordS_I = TBD.order.findOrder(pair.atomI, orderKey);
		auto ordS_J = TBD.order.findOrder(pair.atomJ, orderKey);
		if( !ordS_I.first )	return;
		if( !ordS_J.first )	return;
		
		normalizeXVec(ordS_I.second);
		normalizeXVec(ordS_J.second);
		
		SuperExchangeList.push_back(boost::make_tuple(pair.atomI, ordS_I.second, ordS_J.second, Jsx, orderKey));
	}
	void addDMExchange		(string opt, string svar)		{ // svar === " @:cspin * Jdm "
		replaceAll(opt, "\t", " ");
		auto  dummyParser = split(opt, " ");
		if( dummyParser.size() != 1){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		
		removeSpace(opt);
		auto parser = split(opt, ":");
		if( parser.size() != 3){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		
		auto pair = TBD.Lat.getPair(parser[2]);
		
		if( pair.atomI.atomName != parser[0])	return;
		if( pair.atomJ.atomName != parser[1])	return;
		if( !pair.withinRange() )				return;
		
		removeSpace(svar);
		auto svarParser = split(svar, "*");
		auto orderKey = svarParser[0];
		
		auto ordS_I = TBD.order.findOrder(pair.atomI, orderKey);
		auto ordS_J = TBD.order.findOrder(pair.atomJ, orderKey);
		if( !ordS_I.first )	return;
		if( !ordS_J.first )	return;
		
		normalizeXVec(ordS_I.second);
		normalizeXVec(ordS_J.second);
		
		x_mat Dij(1,3);
		r_mat bondIJ = pair.bondIJ();
		Dij[0] = bondIJ[0];
		Dij[1] = bondIJ[1];
		Dij[2] = bondIJ[2];
		Dij = Dij * (1/abs(cdot(Dij, Dij)));
		
		if	( svarParser.size() == 2){
			auto tmpXVec = TBD.parseSiteString(pair.atomI, svarParser[1]);
			if		( tmpXVec.size() == 1 ){ Dij = Dij * tmpXVec[0].real(); }
			else if	( tmpXVec.size() == 3 ){ Dij = tmpXVec; }
		}
		
		DMExchangeList.push_back(boost::make_tuple(pair.atomI, ordS_I.second, ordS_J.second, Dij, orderKey));
	}
	void addFieldB			(string opt, string svar)		{ // svar === " @:cspin * Jse "
		replaceAll(opt, "\t", " ");
		auto  optParser = split(opt, " ");
		if( optParser.size() > 2){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		if( optParser.size() == 2){ return; }
		
		removeSpace(optParser[0]);
		auto parser = split(optParser[0], ":");
		if( parser.size() != 1){ ErrorMessage("Error, not a valid operation:\n"+opt); }
		
		auto atomI = TBD.Lat.getAtom();
		if( atomI.atomName != optParser[0])	return;
		
		removeSpace(svar);
		auto svarParser = split(svar, "*");
		auto vectorB = TBD.parseSiteString(atomI, svarParser[0]);
		r_var multiplyB = 1.0;
		if( svarParser.size() == 2) multiplyB = TBD.parseSiteString(atomI, svarParser[1])[0].real();
		
		vectorB = vectorB * multiplyB;
		
		auto ordS = TBD.order.findOrder(atomI, "@:cspin");
		if( !ordS.first ) return;
		FieldBList.push_back(boost::make_tuple(atomI, ordS.second, vectorB));
	}
private:
	TBDataSource & TBD;
	
	vector<boost::tuple<Atom, x_mat, r_var, string> >			HundCouplingList;
	vector<boost::tuple<Atom, x_mat, x_mat, r_var, string> >	SuperExchangeList;
	vector<boost::tuple<Atom, x_mat, x_mat, x_mat, string> >	DMExchangeList;
	vector<boost::tuple<Atom, x_mat, x_mat> >					FieldBList;
public:
	
	void	constructHamList()								{
		TBD.initOrder();
		HundCouplingList.clear();
		SuperExchangeList.clear();
		DMExchangeList.clear();
		FieldBList.clear();
		
		//TBD.calculate4DensityOrder();
		
		while( TBD.Lat.iterate() ){
			
			if( TBD.Lat.parameter.VAR("disable_quantum", 0).real() == 0 )
			for( auto & iter : TBD.Lat.hamParser.getOperationList("hundSpin"))	addHundCoupling(iter.first, iter.second);
			
			for( auto & iter : TBD.Lat.hamParser.getOperationList("superEx"))	addSuperExchange(iter.first, iter.second);
			for( auto & iter : TBD.Lat.hamParser.getOperationList("dmEx")	)	addDMExchange(iter.first, iter.second);
			for( auto & iter : TBD.Lat.hamParser.getOperationList("fieldB"))	addFieldB(iter.first, iter.second);
		}
	}
	
	void	calculateClassicalEnergy()						{
		
		x_var SE_Energy = 0, DM_Energy = 0, FB_Energy = 0;
		// Calculate Super Exchange energy.
		for ( auto & elem :SuperExchangeList)	{
			auto si = elem.get<1>();
			auto sj = elem.get<2>();
			auto Js = elem.get<3>();
			
			SE_Energy += Js * cdot(si,sj);
		}
		
		// Calculate DM Exchange energy.
		for ( auto & elem :DMExchangeList)		{
			auto si = elem.get<1>();
			auto sj = elem.get<2>();
			auto d = elem.get<3>();

			DM_Energy += cdot(d, curl(si, sj));
		}
		
		for ( auto & elem :FieldBList )	{
			auto si = elem.get<1>();
			auto Bi = elem.get<2>();
			
			FB_Energy += cdot(Bi,si).real();
		}
		
		
		// Calculate the coulomb energy = -\sum_i Coulomb_i * Den_i
		double coulombEnergy = 0;
		
		while( TBD.Lat.iterate() ){
			
			auto parameter_den		= TBD.order.findOrder(TBD.Lat.getAtom(),	"@:den");
			auto parameter_coulomb	= TBD.order.findOrder(TBD.Lat.getAtom(),	"@:coulomb");
			
			if( parameter_den.first and parameter_coulomb.first ){
				coulombEnergy -= parameter_den.second[0].real() * parameter_coulomb.second[0].real();
			}
		}
		
		TBD.energyMap["2.Coul Eng"] = coulombEnergy;
		TBD.energyMap["3.SE Eng"] = SE_Energy.real();
		TBD.energyMap["4.DM Eng"] = DM_Energy.real();
		TBD.energyMap["5.FB Eng"] = FB_Energy.real();
	}
	
	double iterateSpinOrder(OrderParameter & newOrder)		{
		
		if( TBD.Lat.parameter.VAR("isCalculateVar", 0).real() == 0 ){ return 0; }
		
		if( TBD.Lat.parameter.VAR("disable_quantum", 0).real() == 0 ){
			TBD.calculate4DensityOrder();
		}
		
		
		map<unsigned, pair<x_mat, string> > Field;
		
		// Construct the Field-Term for variational method of HundCoupling.
		if( TBD.Lat.parameter.VAR("disable_quantum", 0).real() == 0 )
		for ( auto & elem :HundCouplingList)	{
			auto atomI = elem.get<0>();
			auto si = elem.get<1>();
			auto Jh = elem.get<2>();
			auto optKey = elem.get<3>();
			
			x_mat Si(1,3);
			if (si.size() == 3 ) Si = Jh * si;
			
			if( Field.find(atomI.atomIndex) == Field.end() ) {
				Field[atomI.atomIndex] = make_pair(x_mat(1,3), optKey);
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first + Si;
			}
			else{
				if( Field[atomI.atomIndex].second != optKey ){
					ErrorMessage("Error, ambiguous definition of the force order for:\n"
								 +Field[atomI.atomIndex].second+" and "+optKey+".\n");
				}
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first + Si;
			}
		}
		
		// Construct the Field-Term for variational method of Super Exchange.
		for ( auto & elem :SuperExchangeList)	{
			auto atomI = elem.get<0>();
			auto si = elem.get<1>();
			auto sj = elem.get<2>();
			auto Js = elem.get<3>();
			auto optKey = elem.get<4>();
			
			x_mat sumSj(1,3);
			if (sj.size() == 3) sumSj = Js * sj;
			
			if( Field.find(atomI.atomIndex) == Field.end() ) {
				Field[atomI.atomIndex] = make_pair(x_mat(1,3), optKey);
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first - sumSj;
			}
			else{
				if( Field[atomI.atomIndex].second != optKey ){
					ErrorMessage("Error, ambiguous definition of the force order for:\n"
								 +Field[atomI.atomIndex].second+" and "+optKey+".\n");
				}
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first - sumSj;
			}
		}
		
		// Construct the Field-Term for variational method of DM Exchange.
		for ( auto & elem :DMExchangeList)		{
			auto atomI = elem.get<0>();
			auto si = elem.get<1>();
			auto sj = elem.get<2>();
			auto d = elem.get<3>();
			auto optKey = elem.get<4>();
			
			x_mat sumDj(1,3);
			if (sj.size() == 3 and d.size() == 3){
				sumDj[0] = - d[1]* sj[2] + d[2]* sj[1] ;
				sumDj[1] = - d[2]* sj[0] + d[0]* sj[2] ;
				sumDj[2] = - d[0]* sj[1] + d[1]* sj[0] ;
			}
			
			if( Field.find(atomI.atomIndex) == Field.end() ) {
				Field[atomI.atomIndex] = make_pair(x_mat(1,3), optKey);
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first - sumDj;
			}
			else{
				if( Field[atomI.atomIndex].second != optKey ){
					ErrorMessage("Error, ambiguous definition of the force order for:\n"
								 +Field[atomI.atomIndex].second+" and "+optKey+".\n");
				}
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first - sumDj;
			}
		}
		
		// Construct the Field-Term for variational method of Super Exchange.
		for ( auto & elem :FieldBList )			{
			auto atomI = elem.get<0>();
			auto si = elem.get<1>();
			auto Bi = elem.get<2>();
			
			if( Field.find(atomI.atomIndex) == Field.end() ) {
				Field[atomI.atomIndex] = make_pair(x_mat(1,3), "@:cspin");
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first - Bi;
			}
			else{
				Field[atomI.atomIndex].first = Field[atomI.atomIndex].first - Bi;
			}
		}
		
		// The LLG variational method.
		
		double max_spin_diff = 0;
		
		double variational_dt	= TBD.Lat.parameter.VAR("var_dt", 0.01).real();
		double LLGdamping		= TBD.Lat.parameter.VAR("LLGdamping", 0.2).real();
		for( auto & fieldI : Field){
			auto atomI	= TBD.Lat.getAtom(fieldI.first);
			auto FI		= fieldI.second.first;
			auto orderKeyI	= fieldI.second.second;
			auto orderI = newOrder.findOrder(atomI, orderKeyI);
			
			if( orderI.first) {
				normalizeXVec(orderI.second);
				auto Si = orderI.second;
				
				x_mat Force = curl(Si, FI);			// force
				x_mat dampForce=curl( Force, Si);	// damping force
				
				x_mat Snew =Si	+	Force * variational_dt
								+	dampForce * variational_dt * LLGdamping;
				
				normalizeXVec(Snew);
				
				x_mat Sdiff = Si - Snew;
				double spin_diff = abs(cdot(Sdiff,Sdiff));
				
				if( spin_diff > max_spin_diff ) max_spin_diff = spin_diff;
				
				newOrder.set(atomI.atomIndex, orderKeyI, Snew);
			}
		}
		
		
		//if( TBD.Lat.parameter.VAR("disable_quantum", 0).real() == 0 ){
		//	while( TBD.Lat.iterate() ){
		//		auto parameter_old = TBD.order_old.findOrder(TBD.Lat.getAtom(), "@:den");
		//		auto parameter_new = newOrder.findOrder(TBD.Lat.getAtom(), "@:den");
		//		if( parameter_old.first and parameter_new.first ){
		//			auto den_diff = abs(parameter_old.second[0].real() - parameter_new.second[0].real());
		//			if( max_den_diff < den_diff ) max_den_diff = den_diff;
		//		}
		//	}
		//}
		
		newOrder.save();
		
		calculateClassicalEnergy();
		TBD.calculateEnergy();
		
		return max_spin_diff;
	}

private:
};























