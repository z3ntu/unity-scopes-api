Release notes
=============

Changes in version 0.4.4
========================
  - The register_annotation() method of SearchReply is now deprecated - push(Annotation const&) should
    be used instead. The display order of annotations with respect to results and categories
    got updated in the documentation of that method.

  - Simplified configuration with sensible defaults for all values.

  - scoperunner, scoperegistry, and smartscopesproxy are now install in /usr/lib/<arch>
    (instead of in a subdirectory of <arch>).

  - Runtime::run_scope() now has an overload to accept a path to Runtime.ini. If no file name is passed,
    the system-wide Runtime.ini is used.

  - UNIX domain sockets for Zmq are now placed under /user/run/<uid>/zmq by default.

Changes in version 0.4.2
========================

  - Made the scope search, activate, perform_action, and preview methods non-blocking.
    A (fake) QueryCtrl is returned immediately from these methods now. Calling cancel() before
    the server has finished creating the query remembers the cancel and sends it to the
    server once the server has returned the real QueryCtrl. This change should be transparent
    to application code (the only difference being that these methods complete faster now).

  - CannedQuery class can now be converted to and from a scopes:// uri with to_uri() and from_uri() methods.
    These methods replace to_string() and from_string() methods that got removed.

Changes in version 0.4.0
========================

  - Re-factored proxy class implementation. These changes are API compatible, but not ABI compatible.

  - Renaming of various API elements for consistency and clarity:

    PreviewWidget::add_attribute() -> PreviewWidget::add_attribute_value()
    PreviewWidget::attributes() -> PreviewWidget::attribute_values()
    PreviewWidget::add_component() -> PreviewWidget::add_attribute_mapping()
    PreviewWidget::components() -> PreviewWidget::attribute_mappings()

    ActivationListener -> ActivationListenerBase
    ActivationListenerBase::activation_response() -> ActivationListenerBase::activated()

    PreviewListener -> PreviewListenerBase

    SearchListener -> SearchListenerBase

    PreviewQuery -> PreviewQueryBase

    SearchQuery -> SearchQueryBase

    ActivationBase -> ActivationQueryBase

    ReplyBase -> Reply

    RegistryBase -> Registry

    Query -> CannedQuery
    CannedQuery::scope_name() -> CannedQuery::scope_id()

    ScopeMetadata::scope_name() -> CannedQuery::scope_id()

    Scope::create_query() -> Scope::search()

    ScopeBase::create_query() -> ScopeBase::search()

    SearchQuery::create_subquery() -> SearchQuery::subsearch()

    Variant::Type: changed ordinal values of enumerators

Changes in version 0.3.2
========================
- ActivationResponse::set_scope_data(Variant const&) and scope_data() methods
  have been added; they are meant to replace setHints() and hints() and use
  Variant instead of VariantMap for arbitrary scope data.
  The existing ActivationResponse::setHints(VariantMap const&) and hints()
  methods have been marked as deprecated and for removal in 0.4.0.

Changes in version 0.3.1
========================
- Scope::activate_preview_action() and ScopeBase::activate_preview_action() were
  renamed to perform_action(). They now also require widget identifier
  along with action id and hints.
- Added SearchMetadata and ActionMetadata classes; these classes are now passed
  to create_query(), activate(), perform_action(), preview() methods of
  ScopeBase and Scope (ScopeProxy) instead of a plain VariantMap.
- The 'Handled' state was removed from ActivationResponse::Status and two new
  values were added instead: ShowDash and HideDash.
- Annotation API changes: annotations of 'Card' type were removed and Annotation
  doesn't support category attribute anymore. SearchReply::push() method for
  annotations was renamed to register_annotation(). Annotations are now going to
  be displayed in the order they were registered by scopes.
- Result::activation_scope_name() method was renamed to target_scope_proxy() and
  it now returns ScopeProxy instead of a string. Client code can now use that
  proxy for result activation or preview calls, instead of having to do an extra
  registry lookup.

Changes in version 0.3.0
========================

- Preliminary API for filters has been added via OptionSelectorFilter and
  FilterState classes. This part of the API is not yet supported by Unity shell
  and should be considered highly experimental. 

- ScopeBase::create_query() method now takes Query object instance instead of
  just a search query string. Search query string is now encapsulated in the
  Query class and can be retrieved via Query::query_string().

- ScopeProxy class provides overloaded create_query methods for passing filter
  state and department id. Note: departments are not yet supported across the
  API.

- The scoperegistry allows extra scopes to be added on the command line now:

    $ scoperegistry some/path/Runtime.ini some/other/path/Fred.ini Joe.ini

  This loads Fred and Joe scopes in addition to any scopes picked up
  via the normal configuration. If Fred or Joe appear in configuration
  as well as on the command line, the config file on the command line
  takes precedence. The .so for additional scopes is expected to be
  in the same directory as the corresponding .ini file.