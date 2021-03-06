// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef SUBSCRIPTIONOBJECT_H
#define SUBSCRIPTIONOBJECT_H

#include "TodaySchema.h"

namespace graphql::today::object {

class Subscription
	: public service::Object
{
protected:
	Subscription();

public:
	virtual service::FieldResult<std::shared_ptr<Appointment>> getNextAppointmentChange(service::FieldParams&& params) const;
	virtual service::FieldResult<std::shared_ptr<service::Object>> getNodeChange(service::FieldParams&& params, response::IdType&& idArg) const;

private:
	std::future<response::Value> resolveNextAppointmentChange(service::ResolverParams&& params);
	std::future<response::Value> resolveNodeChange(service::ResolverParams&& params);

	std::future<response::Value> resolve_typename(service::ResolverParams&& params);
};

} /* namespace graphql::today::object */

#endif // SUBSCRIPTIONOBJECT_H
